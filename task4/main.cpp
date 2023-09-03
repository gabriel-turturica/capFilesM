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
#include <TApplication.h>
#include <TCanvas.h>

using namespace std;
using json = nlohmann::json;

int main() {

    ifstream config_file("config.json");
    json config = json::parse(config_file); // parsing the config file
    json detector_types = config["detector"]["type"]; // getting the detector types
    json detector_luts = config["detector"]["lut"]; // getting the detector luts
    config_file.close();
    
    int fd = open("data.cap", O_RDONLY); // open the file and get the file descriptor

    kj::FdInputStream fdStream(fd);
    kj::BufferedInputStreamWrapper bufferedStream(fdStream);
    
    map<pair<int, int>, TH1I*> energyHistograms;
    map<pair<int, int>, TH1I*> timeDifferenceHistograms;
    map<pair<int, int>, TH2I*> energyTimeHistograms; // Two-dimensional histogram map
    map<pair<int,int>, uint64_t> previousTimeStamps; //previous timestamp hashmap
    
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

            // Fill energy histogram
            if(energyHistograms.find(typeLutPair) == energyHistograms.end()) {
                // Create if not created
                energyHistograms[typeLutPair] = new TH1I(Form("EnergyHist_%d_%d", type, lut), 
                                    Form("Energy Histogram - Type %d, Lut %d", type, lut), 16384, 0, 16383);
            }
            // Fill it

            energyHistograms[typeLutPair]->Fill(energy);

            if(previousTimeStamps.find(typeLutPair) != previousTimeStamps.end()) {

                if (energyTimeHistograms.find(typeLutPair) == energyTimeHistograms.end()) {
                    // Create energy-time histogram
                    energyTimeHistograms[typeLutPair] = new TH2I(Form("TimeEnergyHist_%d_%d", type, lut),
                                        Form("Time-Energy Histogram - Type %d, Lut %d", type, lut),
                                        16384, 0, 16383, 500, 0, 0);  // Time on y-axis, Energy on x-axis
                }
                
                // Fill energy-time histogram
                energyTimeHistograms[typeLutPair]->Fill(energy, timestamp - previousTimeStamps[typeLutPair]);

                if(timeDifferenceHistograms.find(typeLutPair) == timeDifferenceHistograms.end()) {
                    //Create time difference histogram
                    timeDifferenceHistograms[typeLutPair] = new TH1I(Form("TimeDiffHist_%d_%d", type, lut),
                                            Form("Time Difference Histogram - Type %d Lut %d", type, lut),
                                            1000, 0 , 0);
                }

                //Fill it
                timeDifferenceHistograms[typeLutPair]->Fill(timestamp - previousTimeStamps[typeLutPair]);
            }
            previousTimeStamps[typeLutPair] = timestamp;
        }
    }

    TFile outputFile("output.root", "RECREATE");
    for(const auto& histPair : energyHistograms) {
        histPair.second->Write();
        delete histPair.second; 
    }
    for(const auto& histPair : timeDifferenceHistograms) {
        histPair.second->Write();
        delete histPair.second;
    }
    for (const auto& histPair : energyTimeHistograms) {
        histPair.second->Draw("colz"); //Colors the histogram before displaying it
        histPair.second->Write();
        delete histPair.second;
    }
    outputFile.Close();
    

    
    return 0;
}
