#pragma once

#include <cmath>
#include <string>
#include <iomanip>
#include <iostream>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/date_facet.hpp>

namespace pt = boost::posix_time;

namespace timez {
  pt::ptime now();

  /*RFC3399Nano returns a string representaton of the passed time.. EG like
   * 2006-01-02T15:04:05.999999999Z07:00
   * */
  std::string RFC3399Nano( pt::ptime );


  /*spinlock does nothing for the time asked for (in ms)
   * */
  void spinlock( int ms );
};


namespace sample {
  /* sample is a sample in time representing some sort of wx station measurement. */
  class sample {
    public:
      sample(): pressure( NAN ), ptemperature( NAN ), humidity( NAN ), htemperature( NAN ), airtemperature( NAN ), when( timez::now() ) {} //broken values
      std::string json();
      std::string  csv();
      double pressure;
      double ptemperature;
      double humidity;
      double htemperature;
      double airtemperature;
    private:
      std::string nullforNan( double );
      pt::ptime when;
  };
}
