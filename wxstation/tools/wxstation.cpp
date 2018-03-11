#include <boost/program_options.hpp>
#include <iostream>
#include <iterator>

#include <wxstation.hpp>

namespace po = boost::program_options;
using namespace std;

int main( int ac, char *av[] ) {
  try {
    // Declare three groups of options.
    po::options_description opts( "Fiddly bits" );
    opts.add_options()
    ( "help", "Produce this glorious help message" )
    ( "version", "output the version number" )
    ( "id", po::value<std::string>()->default_value( "" ) , "Full named of the 1-Wire DS18B20 senosr that would show up under the /sys/bus/w1/drivers/w1_slave_driver folder as a something named 28-0316756c4cff or similar.  An empty value (default) will attempt to locate the first sensor and use that." )
    ( "fmt", po::value<std::string>()->default_value( "csv" ) , R"(Output format.  Only "csv" or "json" supported. )" )
    ( "delay", po::value<int>()->default_value( 0 ) , R"(How long to wait after the previous sample before attempting another, in seconds)" )
    ( "i2c", po::value<std::string>()->default_value( "/dev/i2c-2" ) , R"(The i2c device the Baromoter and Hyrgometer are on)" )
    ;

    po::variables_map vm;
    po::store( po::parse_command_line( ac, av, opts ), vm );

    if ( vm.count( "help" ) ) { cout << opts; return 0; }
    if ( vm.count( "version" ) ) { cout << GIT_TAG << std::endl; return 0; }

    int delay = vm["delay"].as<int>();
    bool csv = vm["fmt"].as<std::string>() == "csv";
  
    wxstation::WxStation station(
        vm["i2c"].as<string>(),
        vm["id"].as<string>());
  
    while ( 1 ) {
      wxstation::sample sam = station.Measure();
      std::cout << (csv ? sam.csv(): sam.json()) << std::endl;
      if (delay) {
        sleep(delay);
      }
    }

  } catch ( std::exception &e ) {
    cout << e.what() << "\n";
  }
}


