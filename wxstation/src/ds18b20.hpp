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

#pragma once

#include <iostream>
#include <fstream>      // std::stringstream, std::stringbuf
#include <string>
#define BOOST_FILESYSTEM_NO_DEPRECATED 1
#include <boost/filesystem.hpp>
#include <iostream>

#include "helpers.hpp"

namespace fs = boost::filesystem;

namespace wxstation {

/*
class DS18B20 wraps around the /sys/bus/w1/driver/w1_slave_driver interface
and reads temperature values from a DS18B20. This class is dumb.  It expects only
one temp sensor, and blindly parses what it is looking for.
*/
class DS18B20 {
public:
  DS18B20(std::string sensorID = "", std::string w1root = "/sys/bus/w1/drivers/w1_slave_driver"): id(sensorID), root(w1root) {
    if (id == "") detect();
  }
  void Initiate(); //start reading data by opening file
  int Sample( sample &samp ); //patch sample in

private:
  void detect(); //looks for a the first 1wire temp device
  std::string id;
  fs::path root;
  std::ifstream dev;
};


}; //namespace wxstation
