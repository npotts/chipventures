#include "si7021.hpp"

namespace pt = boost::posix_time;

namespace SI7021 {

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
    char config[] = {WRITE_USR_REG, regVal};
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
    char discarded[] = {MEAS_RH_NOHOLD, 0};
    write( i2cfd, discarded, 1 );
    sampleInit = timez::now();
  }

  /*A Sample takes at least 23 ms  ~10.8 for the T, 12 for RH*/
  int SI7021::Sample( sample::sample &samp ) {
    char d[] = {0, 0, 0};
    pt::ptime start;
    pt::time_duration max = pt::milliseconds( 50 );
    int rd;

    // Read the data
    select();
    start = timez::now();
    while ( timez::now() - start < max && rd != 2 ) {
      rd = read( i2cfd, d, 2 );
      if ( rd == 2 ) {
        samp.humidity = ( ( ( d[0] * 256 + d[1] ) * 125.0 ) / 65536.0 ) - 6;
      }
    }


    select();
    rd = 0;
    char w[] = {MEAS_T_NOHOLD};
    start = timez::now();
    while ( timez::now() - start < max && rd != 1 ) {
      rd = write( i2cfd, w, 1 );
    }
    if ( rd != 1 ) {
      return rd;
    }

    rd = 0;
    start = timez::now();
    while ( timez::now() - start < max && rd != 2 ) {
      rd = read( i2cfd, d, 2 );
      if ( rd == 2 ) {
        samp.htemperature = ( ( ( d[0] * 256 + d[1] ) * 175.72 ) / 65536.0 ) - 46.85;
        return 0;
      }
    }
    return rd;
  }
};  // namespace SI7021
