#include <iostream>
#include <string>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include "eventProto.capnp.h"
#include <fcntl.h>
#include <map>
#include <vector>
#include <fstream>
#include "json.hpp"
#include <TH1I.h>
#include <TFile.h>

using namespace std;
using json = nlohmann::json;

int main() {

    // test
        ofstream out("data.out");
    //

    ifstream config_file("config.json");
    json config = json::parse(config_file); // parsing the config file
    json detector_types = config["detector"]["type"]; // getting the detector types
    json detector_luts = config["detector"]["lut"]; // getting the detector luts
    
    int fd = open("data.cap", O_RDONLY); // open the file and get the file descriptor

    kj::FdInputStream fdStream(fd);
    kj::BufferedInputStreamWrapper bufferedStream(fdStream);

    map<pair<int, int>, TH1I*> energyHistograms;
    map<pair<int, int>, TH1I*> timeDifferenceHistograms; // New map for time differences
    
    uint64_t prevTimestamp = 0; // To store the timestamp of the previous event

    while (bufferedStream.tryGetReadBuffer() != nullptr) // while there is data to read
    {
        capnp::PackedMessageReader message(bufferedStream, {100000000, 64});

        // get data reader
        PlainData::Reader reader = message.getRoot<PlainData>();

        // get event list
        auto proEvent = reader.getEvents();
        
        // iterate through the event list
        for (const auto& event : proEvent) {
            uint8_t board = event.getBoard();
            uint8_t channel = event.getChannel();
            uint16_t energy = event.getEnergy();
            uint64_t timestamp = event.getTimestamp(); // Get the timestamp
            
            int type = detector_types[board][channel];
            int lut = detector_luts[board][channel];
            
            pair<int, int> typeLutPair = make_pair(type, lut);
            
            if (energyHistograms.find(typeLutPair) == energyHistograms.end()) {
                // Create energy histogram
                TH1I* hist = new TH1I(Form("EnergyHist_%d_%d", type, lut),
                                      Form("Energy Histogram - Type %d, Lut %d", type, lut),
                                      16384, 0, 16383);  // Assuming 14-bit digitizer
                energyHistograms[typeLutPair] = hist;
            }
            
            // Fill energy histogram
            TH1I* energyHist = energyHistograms[typeLutPair];
            energyHist->Fill(energy);
            
            if (prevTimestamp != 0) {
                // Calculate time difference and fill time difference histogram
                uint64_t timeDifference = timestamp - prevTimestamp;
                out << timeDifference << endl;
                if (timeDifferenceHistograms.find(typeLutPair) == timeDifferenceHistograms.end()) {
                    TH1I* timeDiffHist = new TH1I(Form("TimeDiffHist_%d_%d", type, lut),
                                                  Form("Time Difference Histogram - Type %d, Lut %d", type, lut),
                                                  1000, 0, 999);
                    timeDifferenceHistograms[typeLutPair] = timeDiffHist;
                }
                TH1I* timeDiffHist = timeDifferenceHistograms[typeLutPair];
                timeDiffHist->Fill(timeDifference);
            }
            
            prevTimestamp = timestamp; // Update the previous timestamp
        }
    }

    TFile outputFile_energies("output_energies.root", "RECREATE");
    for (const auto& histPair : energyHistograms) {
        histPair.second->Write();
        delete histPair.second;
    }
    outputFile_energies.Close();

    TFile outputFile_timeDiffs("output_timeDiffs.root", "RECREATE");
    for (const auto& histPair : timeDifferenceHistograms) {
        histPair.second->Write();
        delete histPair.second;
    }
    outputFile_timeDiffs.Close();

    config_file.close(); out.close();
    return 0;
}
