The tasks for task1 are in the task1/README.md

Update : Below is a summary of the three tasks:

The first task requires reading data.cap, some code was already added to main.cpp. Use the Makefile to compile the code.

Task 1:

  1. compile eventProto.capnp (check the documentation https://capnproto.org/)
  2. finish the code from main.cpp
  3. store the number of counts asociated with each board and channel in some data structure
  4. print the number of counts

The second task requires reading a json config file and validate that the stored data (data.cap) coresponds to the configuration file of the experiment.

Task 2:

  1. use nlohmann json library to parse the configuration file config.json (https://github.com/nlohmann/json)
  2. the "detector" section of the configuration file section contains the detector TYPEs associated with the experiment. A detector TYPE of -1 means no detector enabled for the associated board/channel (type[0] -> board0, type[0][0] -> channel0 of board0).
  3. using the code from task1 validate that data.cap contains the data associated with the configuration file. Raise an error if data.cap contains data associated with a TYPE=-1 in the config. Raise an error if any TYPE different from zero lacks data in data.cap.

The third task is based around generating histograms for energy and time differences.

Task 3:

  1. Install ROOT CERN library (build from source https://root.cern/install/build_from_source/)
  2. Construct a map for the energies using the ROOT TH1I histograms (map<pair<type,lut>, TH1I>, the type and lut are converted using the board / channel) filled with the energy for each event. (hint: the energy was measured with a 14 bit digitizer)
  3. Construct a map for the time differences (map<pair<type,lut>, TH1I>) using the ROOT TH1I histograms. The histograms should be filled with the differences in timestamps from consecutive events: event[i+1] - event[i].
  4. Save all the histograms to an ROOT output file.
