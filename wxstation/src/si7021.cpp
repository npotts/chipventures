#include "si7021.hpp"


namespace pt = boost::posix_time;

namespace SI7021 {

  /*This creates one with some useful values*/
  HumiditySample::HumiditySample( char rhMSB, char rhLSB, char tempMSB, char tempLSB ): HumiditySample() {
    humidity = ( ( ( rhMSB * 256 + rhLSB ) * 125.0 ) / 65536.0 ) - 6;
    temperature = ( ( ( tempMSB * 256 + tempLSB ) * 175.72 ) / 65536.0 ) - 46.85;
  }

  std::string HumiditySample::JSON() {
    std::stringstream ss;
    ss << "{ \"when\":\"" << timez::RFC3399Nano( when ) << "\",\"humidity\":" << humidity << ",\"temperature\":" << temperature << "}" << std::endl;
    return ss.str();
  }

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
  SI7021::SI7021( int fd, char regVal ): i2cfd( fd ), reg( regVal ), sampleInit()  {
    select();
    char config[] = {WRITE_USR_REG, regVal};
    write( i2cfd, config, 2 ); //set register
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
    char discarded[] = {MEAS_RH_NOHOLD, 0};
    std::cout << "RD" << write( i2cfd, discarded, 1 ) << std::endl; //read (and discard) from CTRL_REG1 sand then issue start
    sampleInit = timez::now();
  }

  /*A Sample takes at least 23 ms  ~10.8 for the T, 12 for RH*/
  HumiditySample SI7021::Sample() {
    int rd;
    pt::time_duration atleast = pt::milliseconds( 200 );
    while ( 1 ) {
      pt::time_duration elapsed = ( timez::now() - sampleInit );
      if ( elapsed > atleast ) break;
    }
    select();

    //Read the data
    char d[] = {0, 0, 0, 0};
    rd = read(i2cfd, d, 2);
    if (rd != 2 ) {
      return HumiditySample();
    }

   select(); 
   char w[] = {MEAS_T_NOHOLD};
   rd = -1;
   while (rd != 1) {
      rd = write( i2cfd, w, 1 ); //just fetch the last measured t
   }

    rd = -1;
    while (rd == -1) {
      rd = read(i2cfd, d+2, 2);
    }
    return HumiditySample( d[0], d[1], d[2], d[3] );
  }
};
