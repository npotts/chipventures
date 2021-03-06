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

#include <cmath>
#include <string>
#include <iomanip>
#include <iostream>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/date_facet.hpp>

namespace pt = boost::posix_time;

namespace wxstation {
  /*Gets the current date/time*/
  pt::ptime now();

  /*RFC3399Nano returns a string representaton of the passed time.. EG like
   * 2006-01-02T15:04:05.999999999Z07:00
   * */
  std::string RFC3399Nano( pt::ptime );


  /*spinlock does nothing for the time asked for (in ms)
   * */
  void spinlock( int ms );

  /* sample is a sample in time representing some sort of wx station measurement. */
  class sample {
    public:
      sample(): pressure( NAN ), ptemperature( NAN ), humidity( NAN ), htemperature( NAN ), airtemperature( NAN ), when( now() ) {} //broken values
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
