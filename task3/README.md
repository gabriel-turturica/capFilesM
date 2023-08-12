The third task is based around generating histograms for energy and time differences.

1. Install ROOT CERN library (build from source https://root.cern/install/build_from_source/)
2. Construct a map for the energies using the ROOT TH1I histograms (map<pair<type,lut>, TH1I>, the type and lut are converted using the board / channel) filled with the energy for each event. (hint: the energy was measured with a 14 bit digitizer) 
3. Construct a map for the time differences (map<pair<type,lut>, TH1I>) using the ROOT TH1I histograms. The histograms should be filled with the differences in timestamps from consecutive events: event[i+1] - event[i]. 
4. Save all the histograms to an ROOT output file. 

hints: 
- to plot ROOT histograms inside the c++ applications:
    1. search for TApplication
    2. seach for gPad->WaitPrimitive()

- to check the saved histograms from a ROOT file: 
    1. open terminal
    2. type “root”
    3. press enter
    4. type “new TBrowser()”
    5. Search file in the TBrowser
    6. Double click on the root file in the TBrowser.

