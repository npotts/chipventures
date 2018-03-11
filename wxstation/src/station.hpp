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

#include "helpers.hpp"
#include "mpl3115a2.hpp"
#include "si7021.hpp"
#include "ds18b20.hpp"


namespace wxstation {

int openDevice(std::string path);
  /*WxStation contains the elements of a P, T, and RH measurements.

  Simply create and call Measure to retrieve samples;

  */
class WxStation {
public:
  WxStation(std::string devicepath, std::string onewireID=""): i2cfd(openDevice(devicepath)), hyrgrometer(i2cfd), barometer(i2cfd), thermometer(onewireID) {}
  ~WxStation() {
    close(i2cfd);
  }
  sample Measure();

private:
  int i2cfd;
  SI7021 hyrgrometer;
  MPL3115A2 barometer;
  DS18B20 thermometer;
};

}; //namespace wxstation
