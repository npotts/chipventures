#pragma once

#include <string>
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
  void spinlock(int ms);
};
