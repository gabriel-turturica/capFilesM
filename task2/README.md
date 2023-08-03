The second task requires reading a json config file and validate that the stored data (data.cap) coresponds to the configuration file of the experiment. 

Task 1:
- use nlohmann json library to parse the configuration file config.json (https://github.com/nlohmann/json)
- the "detector" section of the configuration file section contains the detector TYPEs associated with the experiment. A detector TYPE of -1 means no detector enabled for the associated board/channel (type[0] -> board0, type[0][0] -> channel0 of board0).   
- using the code from task1 validate that data.cap contains the data associated with the configuration file. Raise an error if data.cap contains data associated with a TYPE=-1 in the config. Raise an error if any TYPE different from zero lacks data in data.cap. 