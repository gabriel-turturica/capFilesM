#include <iostream>
#include <string>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "task1/eventProto.capnp.h"
#include <fcntl.h>
#include <map>
#include <vector>
#include <fstream>
#include "task2/json.hpp"
using namespace std;
using json = nlohmann::json;
int main()
{
    map<uint8_t, map<uint8_t, int>> boardChannelCounts;
    int fd = open("task1/data.cap", O_RDONLY); // open the file and get the file descriptor
    ofstream out("task1/data.out"); //unnecessary file that stores the same data requested at task1
    ifstream config_file("task2/config.json"); //
    json config = json::parse(config_file); // parsing the config file
    json detector_types = config["detector"]["type"]; //getting the detecotr
    if(!out)
    {
        cout << "Error : could not open output file" << endl;
        return 1;
    }
    
    kj::FdInputStream fdStream(fd);
    kj::BufferedInputStreamWrapper bufferedStream(fdStream);

    while (bufferedStream.tryGetReadBuffer() != nullptr) // while there is data to read
    {
        capnp::PackedMessageReader message(bufferedStream, {100000000, 64});

        // get data reader
        PlainData::Reader reader = message.getRoot<PlainData>();

        // get event list
        auto proEvent = reader.getEvents();

        // iterate through the event list
        for (const auto& event : proEvent)
        {
            // extract and store counts for each board / channel
            uint8_t board = event.getBoard();
            uint8_t channel = event.getChannel();
            boardChannelCounts[board][channel]++;
        }
    }

    // print the counts associated with every channel for each board
    //added a flag for the first type of error
    for (const auto& boardElem : boardChannelCounts) {
        uint8_t board = boardElem.first;
        const auto& channelCounts = boardElem.second;

        out << "Board " << static_cast<int>(board) << ":\n";
        for (const auto& channelElem : channelCounts) {
            uint8_t channel = channelElem.first;
            int count = channelElem.second;
            out << "\tChannel " << static_cast<int>(channel) << " has " << count << " count ";
            int type = detector_types[board][channel];
            if(type == -1 && count != 0) {
                out << "(Error : detector was of -1 type)";
            }
            out << endl;
        }
    }
    out << endl;
    //check for active detectors that didn t count anything
    out << "Further Channels did not count anything, being active : " << endl;
    uint8_t c_board_index = 0;
    for(const auto& configuredBoard : detector_types) {
        uint8_t c_channel_index = 0;
        for(const auto& configuredChannel : configuredBoard)
        {
            int type = static_cast<int>(configuredChannel);
            int count = boardChannelCounts[c_board_index][c_channel_index];
            if(type != -1 && count==0)
            {
                out << "Board " << static_cast<int>(c_board_index) << ", Channel"
                    << static_cast<int>(c_channel_index) << endl;
            }
            c_channel_index++;
        }
        out << endl;
        c_board_index++;
    }
    
    out.close(); config_file.close();
    return 0;
}