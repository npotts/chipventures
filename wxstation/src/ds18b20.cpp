/*
MIT License

Copyright (c) 2018 Nick Potts

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "ds18b20.hpp"

namespace wxstation {

namespace fs = boost::filesystem;

//looks for a the first 1wire temp device
void DS18B20::detect() {
  fs::directory_iterator it{root};
  while (it != fs::directory_iterator{}) {
    if (fs::is_directory(*it) && fs::is_regular_file(*it/"w1_slave")) {
      id = (*it).path().filename().string();
      std::cerr << "DS18B20 - Detected sensor at " << root/id << std::endl;
      break;
    }
    *it++;
  }
}

//start reading data by opening file
void DS18B20::Initiate() {
  if (id == "") detect();
  if (!fs::exists(root/id/"w1_slave")) id = ""; //check for existance. It may disappear
  if (id == "") return; //didnt find a sensor.
  dev.close();
  dev.open(fs::path(root/id/"w1_slave").string());
}

//patch sample in
int DS18B20::Sample( sample &samp ) {
  try {
    /*contents should look like:
      32 01 4b 46 7f ff 0c 10 8f : crc=8f YES
      32 01 4b 46 7f ff 0c 10 8f t=19125
    or similar for a good measurement.  If not, the last line be something like
      32 01 4b 46 7f ff 0c 10 8f : crc=8e NO
    */
    std::string lines[2];
    std::getline(dev, lines[0]);
    std::getline(dev, lines[1]);
    dev.close();
    unsigned int eql = lines[1].find("=");
    if (lines[0].find("YES") == std::string::npos || //bad CRC
        eql == std::string::npos) //malformed output
      return 1;
    samp.airtemperature = std::stod(lines[1].substr(eql+1)) / 1000;
    return 0;
  } catch (const std::exception& e) {
    dev.close();
    std::cerr << "Ouch! this hurts in DS18B20: " << e.what() << std::endl;
    return 1;
  }
}

}; //end namespace wxstation
