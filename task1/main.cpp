#include <iostream>
#include <string>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "eventProto.capnp.h"
#include <fcntl.h>
#include <map>
#include <vector>

using namespace std;

int main()
{
    map<uint8_t, map<uint8_t, int>> boardChannelCounts;
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
            boardChannelCounts[board][channel]++;
        }
    }

    // print the counts associated with every channel for each board
    for (const auto& boardElem : boardChannelCounts) {
        uint8_t board = boardElem.first;
        const auto& channelCounts = boardElem.second;

        cout << "Board " << static_cast<int>(board) << ":\n";
        for (const auto& channelElem : channelCounts) {
            uint8_t channel = channelElem.first;
            int count = channelElem.second;
            cout << "\tChannel " << static_cast<int>(channel) << " has " << count << " count\n";
        }
    }

    return 0;
}
