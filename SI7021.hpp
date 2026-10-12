#ifndef SI7021_HPP
#define SI7021_HPP

#include <hwlib.hpp>

class SI7021 {
private:
  hwlib::i2c_bus& bus; 
public:
  SI7021(hwlib::i2c_bus& bus);
  void reset();
  float readTemperature();
  float readHumidity();
};

#endif // SI7021_HPP
