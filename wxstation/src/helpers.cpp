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

#include "helpers.hpp"

namespace pt = boost::posix_time;
namespace dt = boost::date_time;
namespace wxstation {

  pt::ptime now() {
    return pt::microsec_clock::local_time();
    //    return boost::posix_time::second_clock::local_time();
  }

  /*Returns a RFC3399 representation of a datetime:  EG, it is in this format:
   * 2006-01-02T15:04:05.999999999Z07:00
   **/
  std::string RFC3399Nano( pt::ptime time ) {
    std::stringstream ss;
    pt::time_facet *f = new pt::time_facet( "%Y-%m-%dT%H:%M:%S.%f%z" );
    static std::locale loc( std::cout.getloc(), f ); // loc takes over *f
    ss.imbue( loc );
    ss << time;
    std::string fmted = ss.str();
    return fmted;
  }

  void spinlock( int ms ) {
    auto start = now();
    pt::time_duration atleast = pt::milliseconds( ms );
    while ( 1 ) {
      pt::time_duration elapsed = ( now() - start );
      if ( elapsed > atleast ) return;
    }
  }

  std::string  sample::csv() {
    std::stringstream ss;
    ss << RFC3399Nano( when ) << "," << pressure << "," << ptemperature << "," << humidity << "," << htemperature << "," << airtemperature << std::endl;
    return ss.str();
  }

  std::string sample::nullforNan( double val ) {
    if ( std::isnan( val ) ) return "null";
    std::stringstream ss;
    ss << val;
    return ss.str();
  }

  std::string sample::json() {
    std::stringstream ss;
    ss << R"({"when":" )" << RFC3399Nano(when) << R"( ","pressure":)" << nullforNan( pressure )  << R"(,"ptemperature":)" << nullforNan( ptemperature ) << R"(,"humidity":)" << nullforNan( humidity ) << R"(,"htemperature":)" << nullforNan( htemperature ) << R"(,"airtemperature":)" << nullforNan( airtemperature ) << R"(})" << std::endl;
    return ss.str();
  }

}; //namespace wxstation
