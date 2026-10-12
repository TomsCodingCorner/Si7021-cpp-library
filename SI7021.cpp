#include "SI7021.hpp"

/**
 * @brief Constructor voor de SI7021 klasse.
 * @param bus Referentie naar de I2C-bus voor communicatie met de sensor.
 */
SI7021::SI7021(hwlib::i2c_bus& bus) : bus(bus) {}

/**
 * @brief Reset de SI7021 sensor.
 * Reset de sensor door het verzenden van het reset commando.
 * Wacht vervolgens tot de sensor is gereset.
 */
void SI7021::reset() {
  bus.write(0x40).write(0xFE); // Reset the sensor
  hwlib::wait_ms(15); // Wait for the sensor to reset
}

/**
 * @brief Lees de temperatuur uit.
 * Verzendt het commando voor temperatuurmeting naar de sensor.
 * Wacht tot de meting is voltooid en leest vervolgens de temperatuurwaarde uit.
 * Converteert de ruwe temperatuurwaarde naar graden Celsius.
 * @return De gemeten temperatuur in graden Celsius.
 */
float SI7021::readTemperature() {
  // Publication repair: no-hold measurement, followed by one two-byte read.
  // Si7021-A20 datasheet sections 5 and 5.1; see docs/maintenance.md.
  hwlib::i2c_write_transaction(bus.primitives, 0x40).write(0xF3);
  hwlib::wait_ms(25);

  uint8_t data[2];
  bus.read(0x40).read(data, 2);
  uint8_t tempMSB = data[0];
  uint8_t tempLSB = data[1];

  float temperature = static_cast<float>((tempMSB << 8) | tempLSB);
  temperature = ((temperature * 175.72) / 65536.0) - 46.85;

  return temperature;
}


/**
 * @brief Lees de luchtvochtigheid uit.
 * Verzendt het commando voor het meten van de luchtvochtigheid naar de sensor.
 * Wacht tot de meting is voltooid en leest vervolgens de luchtvochtigheidswaarde uit.
 * Converteert de ruwe luchtvochtigheidswaarde naar procentuele relatieve luchtvochtigheid.
 * @return De gemeten luchtvochtigheid in procentuele relatieve luchtvochtigheid.
 */
float SI7021::readHumidity() {
  hwlib::i2c_write_transaction(bus.primitives, 0x40).write(0xF5); 
  hwlib::wait_ms(25);

  // The no-hold command above already initiates the measurement.
  uint8_t data[2];
  bus.read(0x40).read(data, 2);
  uint8_t RH_CodeMSB = data[0];
  uint8_t RH_CodeLSB = data[1];

  uint16_t RH_Code = (RH_CodeMSB << 8) | RH_CodeLSB; 

  float humidity = (125.0 * RH_Code / 65536.0) - 6.0; 
  
  return humidity;
}


