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
#include <TH2I.h>
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

    map<pair<int, int>, TH2I*> energyTimeHistograms; // Two-dimensional histogram map
    
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
            
            if (energyTimeHistograms.find(typeLutPair) == energyTimeHistograms.end()) {
                // Create energy-time histogram
                TH2I* hist = new TH2I(Form("EnergyTimeHist_%d_%d", type, lut),
                                      Form("Energy-Time Histogram - Type %d, Lut %d", type, lut),
                                      1000, 0, 0, 16384, 0, 16383);  // Time on x-axis, Energy on y-axis
                energyTimeHistograms[typeLutPair] = hist;
            }
            
            // Fill energy-time histogram
            TH2I* energyTimeHist = energyTimeHistograms[typeLutPair];
            energyTimeHist->Fill(timestamp, energy);
        }
    }

    TFile outputFile("output.root", "RECREATE");
    for (const auto& histPair : energyTimeHistograms) {
        histPair.second->Draw("COLZ"); //Colors the histogram before displaying it
        histPair.second->Write();
        delete histPair.second;
    }
    outputFile.Close();
    config_file.close(); out.close();
    return 0;
}
