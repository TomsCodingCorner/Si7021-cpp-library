#ifndef SI7021_HOST_TEST_HWLIB_HPP
#define SI7021_HOST_TEST_HWLIB_HPP
// Host-only hardware shim. Transactions come from the real upstream hwlib
// header supplied on the compiler include path, not a rewritten I2C mock API.
#include <cstdint>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

namespace hwlib {
inline std::vector<std::string> trace;
inline void wait_ms(int ms) { trace.push_back("wait:" + std::to_string(ms)); }
inline void wait_us(int) {}
class noncopyable {
public:
  noncopyable() = default;
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};
class pin_oc {
public:
  virtual void write(bool) {}
  virtual bool read() { return true; }
  virtual void flush() {}
  virtual void refresh() {}
};
inline std::ostream& cout = std::cout;
inline std::ostream& endl(std::ostream& stream) { return std::endl(stream); }
namespace target {
enum class pins { scl, sda, d10, d11, d12 };
class pin_oc : public hwlib::pin_oc {
public:
  explicit pin_oc(pins) {}
};
class pin_in {
public:
  explicit pin_in(pins) {}
  void pullup_enable() {}
  bool read() { return true; }
};
}
}
#include <hwlib-i2c.hpp>
#endif
