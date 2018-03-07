#include "helpers.hpp"

namespace pt = boost::posix_time;
namespace dt = boost::date_time;
namespace timez {

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
};  // namespace timez



namespace sample {
  std::string  sample::csv() {
    std::stringstream ss;
    ss << timez::RFC3399Nano( when ) << "," << pressure << "," << ptemperature << "," << humidity << "," << htemperature << "," << airtemperature << std::endl;
    return ss.str();
  }

  std::string sample::nullforNan( double val ) {
    if ( std::isnan( val ) ) return "null";
    std::stringstream ss;
    ss << std::setprecision( 2 ) << val;
    return ss.str();
  }

  std::string sample::json() {
    std::stringstream ss;
    ss << R"({"when":" )" << timez::RFC3399Nano(when) << R"( ","pressure":)" << nullforNan( pressure )  << R"(,"ptemperature":)" << nullforNan( ptemperature ) << R"(,"humidity":)" << nullforNan( humidity ) << R"(,"htemperature":)" << nullforNan( htemperature ) << R"(,"airtemperature":)" << nullforNan( airtemperature ) << R"(})" << std::endl;
    return ss.str();
  }

};
