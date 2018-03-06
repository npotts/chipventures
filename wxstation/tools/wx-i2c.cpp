#include <cstring> //for strerror
#include <iostream>

//#include <mpl3115a2.hpp>
#include <wxstation.hpp>

namespace {

const char *usage =
    ""
    "Usage:\n"
    "\t./wx-i2c <i2c-device>\n";
}  // namespace

int main(int argc, char *argv[]) {
  if (argc < 2) {
    std::cout << usage << std::endl;
    return 0;
  }

  std::cout << "Attempting to open " << argv[1] << std::endl;
  int i2cfd = open(argv[1], O_RDWR);

  if (i2cfd < 0) {
    std::cout << "Unable to open the bus: " << strerror(errno) << std::endl;
    return 1;
  }
  
  SI7021::SI7021 hyrg(i2cfd);
  //MPL3115A2::MPL3115A2 baro(i2cfd);

  for (int i =0; i< 1000; i++) {
    hyrg.Initiate();
    //baro.Initiate();
    auto hsamp = hyrg.Sample();
    //auto bsamp = baro.Sample();
    std::cout << hsamp.JSON();
    //std::cout << bsamp.JSON();
  }
  close(i2cfd);
}
