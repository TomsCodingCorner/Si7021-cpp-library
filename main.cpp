// Minimal demo adapted from the original testcode.cpp for this publication.
#include "SI7021.hpp"

// hwlib's console does not provide floating-point output here.
// Show hundredths, including the sign for temperatures between -1 and 0 C.
void printHundredths(float value) {
  int scaled = static_cast<int>(value * 100);
  if (scaled < 0) {
    hwlib::cout << '-';
    scaled = -scaled;
  }
  hwlib::cout << scaled / 100 << '.';
  if (scaled % 100 < 10) {
    hwlib::cout << '0';
  }
  hwlib::cout << scaled % 100;
}

int main() {
  auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);
  auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);
  SI7021 sensor(i2c_bus);

  hwlib::cout << "Si7021 demo: reset and one temperature/humidity sample"
              << hwlib::endl;
  hwlib::wait_ms(80); // Datasheet Table 2: maximum power-up time.
  sensor.reset();

  float temperature = sensor.readTemperature();
  float humidity = sensor.readHumidity();
  // A sanity check only: the API cannot report bus errors or verify CRC.
  if (!(temperature >= -40 && temperature <= 125 &&
        humidity >= -1 && humidity <= 101)) {
    hwlib::cout << "Unexpected reading. Check sensor, supply and I2C wiring."
                << hwlib::endl;
    return 1;
  }

  hwlib::cout << "Temperature: ";
  printHundredths(temperature);
  hwlib::cout << " C" << hwlib::endl;
  hwlib::cout << "Humidity: ";
  printHundredths(humidity);
  hwlib::cout << " %RH" << hwlib::endl;
  hwlib::cout << "Sample complete; bus errors and CRC are not checked."
              << hwlib::endl;
  return 0;
}
