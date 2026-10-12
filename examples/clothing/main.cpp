#include <hwlib.hpp>
#include "SI7021.hpp"

int readWholeTemperature(){
  auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);
  auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  SI7021 sensor(i2c_bus);

  float temperature = sensor.readTemperature();
  int temperatureInt = static_cast<int>(temperature * 100);
  return (temperatureInt / 100);
  }


void printHumidity(){
  auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);
  auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);
  auto i2c_bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

  SI7021 sensor(i2c_bus);

  float humidity = sensor.readHumidity();
  int humidityInt = static_cast<int>(humidity * 100); // Convert to integer representation
  hwlib::cout << "Humidity: " << humidityInt / 100 << '%' << hwlib::endl;
  }

void recommendClothing(float temperature) {
  if (temperature < 0) {
    hwlib::cout << "Recommended Clothing: Geïsoleerde jas, muts, sjaal, handschoenen, thermische onderkleding, dikke sokken, gevoerde schoenen." << hwlib::endl;
  } else if (temperature >= 0 && temperature < 10) {
    hwlib::cout << "Recommended Clothing: Warme jas, trui, lange broek, lichte sjaal, gewone sokken." << hwlib::endl;
  } else if (temperature >= 10 && temperature < 20) {
    hwlib::cout << "Recommended Clothing: Licht jack, T-shirt, lange broek of rok." << hwlib::endl;
  } else if (temperature >= 20 && temperature < 30) {
    hwlib::cout << "Recommended Clothing: T-shirt, korte broek of rok, lichte sokken, sandalen of sneakers." << hwlib::endl;
  } else if (temperature >= 30) {
    hwlib::cout << "Recommended Clothing: Luchtige kleding, zoals een T-shirt, korte broek of rok, sandalen, zonnehoed, zonnebril." << hwlib::endl;
  } else {
    hwlib::cout << "Recommended Clothing: N/A" << hwlib::endl;
  }
}

int main() {
    //Button declaration
    auto buttonTempMoist = hwlib::target::pin_in(hwlib::target::pins::d10);
   buttonTempMoist.pullup_enable();

    //Button declaration
    auto buttonAdvice = hwlib::target::pin_in(hwlib::target::pins::d12);
   buttonAdvice.pullup_enable();

    // Stop button
    auto buttonStop = hwlib::target::pin_in(hwlib::target::pins::d11);
   buttonStop.pullup_enable();

   // Use the buttons to print sensor information.
    while(buttonStop.read()){
        float currentTemperature = readWholeTemperature();
        if(!buttonAdvice.read()){
            recommendClothing(currentTemperature);
            }
        else if(!buttonTempMoist.read()){
            hwlib::cout << "Temperatuur: " << readWholeTemperature() << hwlib::endl;
            printHumidity();
            }
        hwlib::wait_ms(20);
        }
    hwlib::cout << hwlib::endl << "Programma afgesloten" << hwlib::endl;

}
