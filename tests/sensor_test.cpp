#include "SI7021.hpp"
#include <cmath>
#include <deque>
#include <sstream>
#include <stdexcept>

// Compile the actual demo's formatting function without running its hardware.
#define main demoMain
#include "../main.cpp"
#undef main

class FakePrimitives : public hwlib::i2c_primitives {
public:
  std::deque<uint8_t> bytes;
  void write_start() override { hwlib::trace.push_back("start"); }
  void write_stop() override { hwlib::trace.push_back("stop"); }
  void write_bit(bool bit) override {
    hwlib::trace.push_back(bit ? "nack" : "ack");
  }
  bool read_bit() override { throw std::runtime_error("Unexpected bit read"); }
  bool read_ack() override { hwlib::trace.push_back("slave-ack"); return true; }
  void write(uint8_t byte) override {
    hwlib::trace.push_back("write:" + std::to_string(byte));
  }
  uint_fast8_t read_byte() override {
    if (bytes.empty()) { throw std::runtime_error("Unexpected extra read"); }
    uint8_t byte = bytes.front();
    bytes.pop_front();
    hwlib::trace.push_back("read:" + std::to_string(byte));
    return byte;
  }
};

void require(bool condition, const char* message) {
  if (!condition) { throw std::runtime_error(message); }
}

int main() {
  try {
    FakePrimitives primitives;
    hwlib::i2c_bus bus(primitives);
    SI7021 sensor(bus);
    sensor.reset();
    require(hwlib::trace == std::vector<std::string>{
      "start", "write:128", "slave-ack", "write:254", "slave-ack",
      "stop", "wait:15"}, "Reset transaction or delay changed");

    for (uint16_t raw : {0x0000, 0x683C, 0x8000, 0xFFFC}) {
      hwlib::trace.clear();
      uint8_t msb = raw >> 8, lsb = raw & 0xFF;
      primitives.bytes = {msb, lsb};
      float value = sensor.readTemperature();
      require(std::fabs(value - (raw * 175.72 / 65536.0 - 46.85)) < 0.0001,
              "Temperature conversion differs from datasheet equation");
      require(hwlib::trace == std::vector<std::string>{
        "start", "write:128", "slave-ack", "write:243", "slave-ack",
        "stop", "wait:25", "start", "write:129", "slave-ack",
        "read:" + std::to_string(msb), "ack", "read:" + std::to_string(lsb),
        "nack", "stop"}, "Temperature sequence, byte order or ACK/NACK incorrect");
    }

    for (uint16_t raw : {0x0002, 0x683E, 0x8002, 0xFFFE}) {
      hwlib::trace.clear();
      uint8_t msb = raw >> 8, lsb = raw & 0xFF;
      primitives.bytes = {msb, lsb};
      float value = sensor.readHumidity();
      require(std::fabs(value - (raw * 125.0 / 65536.0 - 6.0)) < 0.0001,
              "Humidity conversion differs from datasheet equation");
      require(hwlib::trace == std::vector<std::string>{
        "start", "write:128", "slave-ack", "write:245", "slave-ack",
        "stop", "wait:25", "start", "write:129", "slave-ack",
        "read:" + std::to_string(msb), "ack", "read:" + std::to_string(lsb),
        "nack", "stop"}, "Humidity sequence, byte order or ACK/NACK incorrect");
    }

    std::ostringstream output;
    auto* previous = std::cout.rdbuf(output.rdbuf());
    printHundredths(-0.25f);
    hwlib::cout << ' ';
    printHundredths(-12.5f);
    hwlib::cout << ' ';
    printHundredths(5.0f);
    std::cout.rdbuf(previous);
    require(output.str() == "-0.25 -12.50 5.00", "Demo sign or zero padding incorrect");
    std::cout << "PASS: reset, 8 conversions/transaction traces, demo formatting.\n"
              << "Host fake primitives with real hwlib transactions; no hardware tested.\n";
  } catch (const std::exception& error) {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
