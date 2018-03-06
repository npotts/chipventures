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

namespace SI7021 {
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


  /* Barometer Sample represents a Pressure and Temperature sample
   *
  */
  class HumiditySample {
    public:
      HumiditySample(): when( timez::now() ), humidity( NAN ), temperature( NAN ) {} //broken values
      HumiditySample( char rhMSB,  char rhLSB,   char tempMSB,  char tempLSB );
      std::string JSON();
    private:
      pt::ptime when;
      double humidity;
      double temperature;
  };


  /*
   class SI7021 wraps around a i2c file descriptor,
   configurex it */
  class SI7021 {
    public:
      SI7021( int i2cfd, char reg = 0x1A );
      void Initiate(); //starts a sample
      HumiditySample Sample(); //retrieve final sample
    private:
      void select(); //select this I2C device
      int i2cfd;
      char reg;
      pt::ptime sampleInit;
  };
}; //namespace
