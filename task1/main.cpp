#include <iostream>
#include <string>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "eventProto.capnp.h"
#include <fcntl.h>

using namespace std;

int main()
{
    int fd; // open the file and get the file descriptor

    kj::FdInputStream fdStream(fd);
    kj::BufferedInputStreamWrapper bufferedStream(fdStream);

    while (bufferedStream.tryGetReadBuffer() != nullptr) // while there is data to read
    {

        capnp::PackedMessageReader message(bufferedStream, {100000000, 64});

        // get data reader
        PlainData::Reader reader;

        // get event list
        auto proEvent = reader.getEvents();

        // iterate through the event list
        for ()
        {
            // extract and store counts for each board / channel
        }
    }

    // print the counts for each board / channel
}