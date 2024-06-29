#include <iostream>
#include <string>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "eventProto.capnp.h"
#include <fcntl.h>
#include <map>

using namespace std;
int main()
{
    map <uint8_t, int> boardCounts, channelCounts;
    int fd = open("data.cap", O_RDONLY); // open the file and get the file descriptor

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
            if (boardCounts.find(board) != boardCounts.end()) {
                boardCounts[board]++;
            } else {
                boardCounts[board] = 1;
            }

            if (channelCounts.find(channel) != channelCounts.end()) {
                channelCounts[channel]++;
            } else {
                channelCounts[channel] = 1;
            }
        }
    }

    // print the counts for each board / channel
    for (const auto& elem : boardCounts) {
        cout << "Board" << (int)elem.first << " has " << elem.second << " count\n";
    }
    for (const auto& elem : channelCounts) {
        cout << "Channel" << (int)elem.first << " has " << elem.second << " count\n";
    }
}