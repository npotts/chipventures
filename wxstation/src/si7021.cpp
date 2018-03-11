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

#include "si7021.hpp"

namespace pt = boost::posix_time;

namespace wxstation {

  /*SI7021 extracts measurements from a i2c bound SI7021 device.


  REG1 definition
  Bit   Symbol - Meaning
  7     RES1   Sample Resolution,  See below
  0     RES0
  6     VDDS  0: ok, 1: Low (read only)
  2     HTRE  0= no heater, 1 = heater

  Remaining are reserved

  Sample Resolution (Bits):
  RES   RH  Temp
  00    12  14
  01     8  12
  10    10  13
  11    11  11

  Overall, we want 0b00011010 = 0x1A as our REG1 value.

  We also set the  SI7021::PT_DATA_CFG registry to 0x07

  */
  SI7021::SI7021( int fd, char regVal ) : i2cfd( fd ), reg( regVal ), sampleInit() {
    select();
    char config[] = {(char) si7021::WRITE_USR_REG, regVal};
    write( i2cfd, config, 2 ); // set register
  }

  /*Make sure the kernel knows we are talking to dev 0x60.*/
  void SI7021::select() {
    // Get I2C device, SI7021 I2C address is 0x40
    int e = ioctl( i2cfd, I2C_SLAVE, 0x40 );
    if ( e != 0 ) {
      throw std::invalid_argument( strerror( errno ) );
    }
  }
  /*Initialize starts the conversion process.
   *
   * One convertion will yield RH & temp.
   * */
  void SI7021::Initiate() {
    select();
    // read (and discard) from MEAS_RH_NOHOLD sand then issue start
    char discarded[] = {(char)si7021::MEAS_RH_NOHOLD, 0};
    write( i2cfd, discarded, 1 );
    sampleInit = now();
  }

  /*A Sample takes at least 23 ms  ~10.8 for the T, 12 for RH*/
  int SI7021::Sample( sample &samp ) {
    char d[] = {0, 0, 0};
    pt::ptime start;
    pt::time_duration max = pt::milliseconds( 50 );
    int rd;

    // Read the data
    select();
    start = now();
    while ( now() - start < max && rd != 2 ) {
      rd = read( i2cfd, d, 2 );
      if ( rd == 2 ) {
        samp.humidity = ( ( ( d[0] * 256 + d[1] ) * 125.0 ) / 65536.0 ) - 6;
      }
    }


    select();
    rd = 0;
    char w[] = {(char) si7021::MEAS_T_NOHOLD};
    start = now();
    while ( now() - start < max && rd != 1 ) {
      rd = write( i2cfd, w, 1 );
    }
    if ( rd != 1 ) {
      return rd;
    }

    rd = 0;
    start = now();
    while ( now() - start < max && rd != 2 ) {
      rd = read( i2cfd, d, 2 );
      if ( rd == 2 ) {
        samp.htemperature = ( ( ( d[0] * 256 + d[1] ) * 175.72 ) / 65536.0 ) - 46.85;
        return 0;
      }
    }
    return rd;
  }
};  // namespace wxstation
