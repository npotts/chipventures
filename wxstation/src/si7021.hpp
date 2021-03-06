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
#include <sstream>      // std::stringstream, std::stringbuf
#include <stdexcept>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <cmath>
#include <iomanip>
#include <cstring> //for strerr
#include <string>

#include "helpers.hpp"

namespace pt = boost::posix_time;

namespace wxstation {

namespace si7021 {
  enum Registers {
    MEAS_RH_HOLD = 0xE5,
    MEAS_RH_NOHOLD = 0xF5,
    MEAS_T_HOLD = 0xE3,
    MEAS_T_NOHOLD = 0xF3,
    READ_LAST_T = 0xE0,
    RESET = 0xFE,
    WRITE_USR_REG = 0xE6,
    READ_USR_REG = 0xE7,
    WRITE_HEATER_REG = 0x51,
    READ_HEATER_REG = 0x11,
    //the following are odd 1 byte cmd codes
    READ_ID_HIGHBYTE_1 = 0xFA,
    READ_ID_HIGHBYTE_2 = 0x0F,
    READ_ID_LOWBYTE_1 = 0xFC,
    READ_ID_LOWBYTE_2 = 0xC9,
    FIRMWARE_1 = 0x84,
    FIRMWARE_2 = 0xB8
  };
}; //end si7021

/*
 class SI7021 wraps around a i2c file descriptor,
 configurex it */
class SI7021 {
  public:
    SI7021( int i2cfd, char reg = 0x1A );
    void Initiate(); //starts a sample
    int Sample( sample &samp ); //retrieve final sample

  private:
    void select(); //select this I2C device
    int i2cfd;
    char reg;
    pt::ptime sampleInit;
}; //end class

}; //namespace wxstation
