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

#include "mpl3115a2.hpp"

namespace pt = boost::posix_time;

namespace wxstation {

  /*MPL3115A2 extracts measurements from a i2c bound MPL3115A2 device.

  REG1 definition
  Bit   Symbol - Meaning
  7     ALT      0=baro (dflt), 1=alt mode
  6     RESERVED
  5:3   Oversample Ratio (see below)
  2     Reset (set to zero)
  1     OST   Initialize measurement immediately.  If SBYB=0, writing a 1 means
  the devie starts a measurement.  Might need to reread this at some point 0 SBYB
  If 1, it auto-measureses into a FIFO buffer. 0 means you


  Oversample Ratio:
  OS2:0  Ratio   Min Sample Time
  000     1      6ms
  001     2      10ms
  010     4      18ms
  011     8      34ms
  100    16      66ms
  101    32     130ms
  110    64     258ms
  111   128     512ms

  Overall, we want 0b00011010 = 0x1A as our REG1 value.

  We also set the  MPL3115A2::PT_DATA_CFG registry to 0x07

  */
  MPL3115A2::MPL3115A2( int fd, char reg1Val )
    : i2cfd( fd ), reg1( reg1Val ), sampleInit() {
    select();
    char config[] = {mpl3115a2::CTRL_REG1, reg1Val};
    write( i2cfd, config, 2 ); // set register

    // set the flag register
    char flags[] = {mpl3115a2::PT_DATA_CFG, 0x07};
    write( i2cfd, flags, 2 );
  }

  /*Make sure the kernel knows we are talking to dev 0x60.*/
  void MPL3115A2::select() {
    // Get I2C device, MPL3115A2 I2C address is 0x60(96)
    int e = ioctl( i2cfd, I2C_SLAVE, 0x60 );
    if ( e != 0 ) {
      throw std::invalid_argument( strerror( errno ) );
    }
  }

  void MPL3115A2::Initiate() {
    select();
    char discarded[] = {mpl3115a2::CTRL_REG1};
    // read (and discard) from CTRL_REG1 sand then issue start
    write( i2cfd, discarded, 1 );

    // discard read, which resets sample state
    read( i2cfd, discarded, 1 );

    char config[] = {mpl3115a2::CTRL_REG1, 0x1A};
    write( i2cfd, config, 2 ); // set registeri
    sampleInit = now();
  }

  int MPL3115A2::Sample( sample &samp ) {
    int rd = 0;
    pt::ptime start;
    pt::time_duration max = pt::milliseconds( 70 );
    pt::time_duration atleast = pt::milliseconds( 335 );
    while ( 1 ) {
      pt::time_duration elapsed = ( now() - sampleInit );
      if ( elapsed > atleast ) break;
    }

    // Read the data
    select();
    start = now();
    char w[] = {mpl3115a2::STATUS};
    while ( now() - start < max && rd != 1 ) {
      rd = write( i2cfd, w, 1 ); // start from the status reg, and read from there to 6:
    }
    if ( rd != 1 ) {
      return rd;
    }

    rd = 0;
    start = now();
    unsigned char d[6] = {0, 0, 0, 0, 0, 0};
    while ( now() - start < max ) {
      rd = read( i2cfd, d, 6 );
      if ( rd == 6 && ( d[0] & 0x08 ) ) {
        samp.pressure = ( double )( ( d[1] * ( 1 << 16 ) ) + ( d[2] * ( 1 << 8 ) ) +
                                    ( d[3] & 0xF0 ) ) / 16.0;
        samp.pressure /= 400.0;  // get it into mBar / hPa, not kPa
        samp.ptemperature = ( double )( ( d[4] * 256 ) + ( d[5] & 0xF1 ) ) / 16.0;
        samp.ptemperature /= 16;  //???
        return 0;
      }
    }
    return rd;
  }
};  // namespace wxstation
