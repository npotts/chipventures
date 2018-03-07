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

namespace MPL3115A2 {
  enum Registers {
    STATUS = 0x00,
    OUT_P_MSB = 0x01,
    OUT_P_CSB = 0x02,
    OUT_P_LSB = 0x03,
    OUT_T_MSB = 0x04,
    OUT_T_LSB = 0x05,
    DR_STATUS = 0x06, //alias for STATUS.  Same contents
    WHO_AM_I = 0x0C,
    PT_DATA_CFG = 0x13, //data ready event flags
    CTRL_REG1 = 0x26,
    CTRL_REG2 = 0x27,
    CTRL_REG3 = 0x28,
    CTRL_REG4 = 0x29,
    CTRL_REG5 = 0x2A
  };

  /*
   class MPL3115A2 wraps around a i2c file descriptor,
   configurex it */
  class MPL3115A2 {
    public:
      MPL3115A2( int i2cfd, char reg1 = 0x1A );
      void Initiate(); //starts a sample
      int Sample( sample::sample &samp ); //retrieve final sample, return non-zero error
    private:
      void select(); //select this I2C device
      int i2cfd;
      char reg1;
      pt::ptime sampleInit;
  };
}; //namespace
