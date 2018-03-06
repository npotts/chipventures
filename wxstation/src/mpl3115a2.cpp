#include "mpl3115a2.hpp"


namespace pt = boost::posix_time;

namespace MPL3115A2 {

  /*This creates one with some useful values*/
  BarometerSample::BarometerSample( unsigned char baroMSB, unsigned char baroCSB, unsigned char baroLSB, unsigned char tempMSB, unsigned char tempLSB ): BarometerSample() {
    pressure = ( double )( ( baroMSB * ( 1 << 16 ) ) + ( baroCSB * ( 1 << 8 ) ) + ( baroLSB & 0xF0 ) ) / 16.0; //
    pressure /= 400.0; // get it into mBar / hPa, not kPa
    temperature = ( double )( ( tempMSB * 256 ) + ( tempLSB & 0xF1 ) ) / 16;
    temperature /= 16; //???
  }

  std::string BarometerSample::JSON() {
    std::stringstream ss;
    ss << "{ \"when\":\"" << timez::RFC3399Nano( when ) << "\",\"pressure\":" << pressure << ",\"temperature\":" << temperature << "}" << std::endl;
    return ss.str();
  }

  /*MPL3115A2 extracts measurements from a i2c bound MPL3115A2 device.

  REG1 definition
  Bit   Symbol - Meaning
  7     ALT      0=baro (dflt), 1=alt mode
  6     RESERVED
  5:3   Oversample Ratio (see below)
  2     Reset (set to zero)
  1     OST   Initialize measurement immediately.  If SBYB=0, writing a 1 means the devie starts a measurement.  Might need to reread this at some point
  0     SBYB  If 1, it auto-measureses into a FIFO buffer. 0 means you


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
  MPL3115A2::MPL3115A2( int fd, char reg1Val ): i2cfd( fd ), reg1( reg1Val ), sampleInit()  {
    select();
    char config[] = {CTRL_REG1, reg1Val};
    write( i2cfd, config, 2 ); //set register

    //set the flag register
    char flags[] = {PT_DATA_CFG, 0x07};
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
    char discarded[] = {CTRL_REG1};
    write( i2cfd, discarded, 1 ); //read (and discard) from CTRL_REG1 sand then issue start
    read( i2cfd, discarded, 1 ); //discard read, which resets sample state

    char config[] = {CTRL_REG1, 0x1A};
    write( i2cfd, config, 2 ); //set registeri
    sampleInit = timez::now();
  }

  BarometerSample MPL3115A2::Sample() {
    pt::time_duration atleast = pt::milliseconds( 335 );
    while ( 1 ) {
      pt::time_duration elapsed = ( timez::now() - sampleInit );
      if ( elapsed > atleast ) break;
    }

    //Read the data
    char w[] = {STATUS};
    write( i2cfd, w , 1 ); //start from the status reg, and read from there to 6:
    char d[6] = {0, 0, 0, 0, 0, 0};
    if ( read( i2cfd, d, 6 ) != 6 ) {
      // throw std::runtime_error("Expected to read 6 characters, and I didnt");
      return BarometerSample();
    }
    return BarometerSample( d[1], d[2], d[3], d[4], d[5] );
  }

};
