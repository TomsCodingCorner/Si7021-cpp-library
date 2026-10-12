# Si7021 C++ Library

A small C++ library I built in **2023** to read temperature and humidity from a Si7021 sensor. It started as a school assignment, but I enjoyed getting the hardware working and continued developing it as a personal project.

I wrote the sensor-specific library using [hwlib](https://github.com/wovo/hwlib) for I²C communication. Alongside the library, I built a small application that uses the readings to suggest what to wear.

In 2026, I revisited the project to make it easier to build and share. This version keeps the original class and public API, with documented protocol fixes, a simpler demonstration, updated build instructions, and automated host tests.

**Originally developed in 2023 · Cleaned up for sharing in October 2026**

## What it does

The library provides three operations:

- Reset the sensor.
- Read temperature in degrees Celsius.
- Read relative humidity as a percentage.

It communicates with the Si7021 at its fixed 7-bit I²C address, `0x40`.

The included examples target the **classic Arduino Uno with an ATmega328P running at 16 MHz**. Communication uses hwlib's software I²C implementation rather than Arduino `Wire`.

This is a small educational library. It does not implement heater control, resolution selection, device identification, CRC validation, or reliable bus-error reporting.

## Quick look

The sensor takes a reference to an existing hwlib I²C bus:

```cpp
auto scl = hwlib::target::pin_oc(hwlib::target::pins::scl);
auto sda = hwlib::target::pin_oc(hwlib::target::pins::sda);
auto bus = hwlib::i2c_bus_bit_banged_scl_sda(scl, sda);

SI7021 sensor(bus);

hwlib::wait_ms(80);
sensor.reset();

float temperature = sensor.readTemperature();
float humidity = sensor.readHumidity();
```

The complete example is in [`main.cpp`](main.cpp).

**Verification status:** both examples have been compiled for the Uno, and the host tests have passed. The updated version has not yet been flashed or tested with a physical sensor.

## Hardware and wiring

For the minimal demo, you will need:

- A classic Arduino Uno.
- A confirmed Si7021 sensor module.
- A USB cable, breadboard, and jumper wires.
- Appropriate power and bidirectional I²C level shifting for your module.

The exact breakout used in the original project is not documented. Check the specifications of your own module before connecting it.

### Proposed demo connections

The following is a proposed setup for the current demo, rather than a verified record of the original circuit.

| Arduino Uno | Si7021 connection | Notes |
| --- | --- | --- |
| SDA / A4 | SDA through a bidirectional I²C level shifter | Match each side to its respective logic supply. |
| SCL / A5 | SCL through a bidirectional I²C level shifter | Match each side to its respective logic supply. |
| GND | Sensor and level-shifter ground | All parts must share a common ground. |
| Regulated 3.3 V | Bare-chip VDD or a documented compatible module input | Check the module's power path and input requirements. |

The bare Si7021-A20 chip operates at **1.9–3.6 V**. Its normal SDA/SCL input range is limited to its supply voltage. A module with a voltage regulator does not necessarily include logic-level conversion.

The datasheet's typical circuit includes pull-up resistors and a decoupling capacitor. Your breakout or level shifter may already provide them; check before adding more.

See the [Silicon Labs Si7021-A20 datasheet](https://cdn-learn.adafruit.com/assets/assets/000/035/931/original/Support_Documents_TechnicalDocs_Si7021-A20.pdf), hosted by Adafruit, and the [Arduino Uno R3 documentation](https://docs.arduino.cc/hardware/uno-rev3/) for electrical details.

## Dependencies

The project uses:

- GNU Make.
- Git.
- AVR-GCC with C++17 support.
- hwlib.
- Additional AVR C++ standard-library headers from `avr-libstdcpp`.
- AVRDUDE for manually uploading firmware.
- A serial terminal for viewing output.

This is a standalone AVR build using hwlib. It does not use the Arduino IDE, Arduino core, or `Wire`.

### Fetch the source dependencies

Run these commands from the repository root:

```sh
git clone https://github.com/wovo/hwlib.git .deps/hwlib
git -C .deps/hwlib checkout d733d8aca4e1fdbd09f2cbb2a0b2390eeb583dcf

git clone https://github.com/modm-io/avr-libstdcpp.git .deps/avr-libstdcpp
git -C .deps/avr-libstdcpp checkout 9d10e0047011f4c1e6ef4dbf76aca5e9a340a6f8
```

These revisions are pinned to the versions used during the publication checks.

### Install the AVR toolchain

The verified build uses **AVR-GCC 11.1.0**, available from the toolchain maintainer's [`v11.1.0-1` release](https://github.com/ZakKemble/avr-gcc-build/releases/tag/v11.1.0-1).

Download the archive for your operating system, verify it against the release's `SHA256SUMS`, extract it, and add its `bin` directory to your PATH.

On Windows, also use a working GNU Make installation, such as one provided through MSYS2.

The tested combination requires the additional `avr-libstdcpp` headers above. Arduino's AVR-GCC 7.3.0 did not build successfully with those headers during the cleanup.

## Build the minimal demo

From the repository root, in PowerShell:

```powershell
New-Item -ItemType Directory -Force .build | Out-Null
avr-g++ --version
make build
```

On a POSIX shell, replace the directory-creation command with:

```sh
mkdir -p .build
```

The build produces:

- `.build/si7021.hex` — firmware for uploading.
- `.build/si7021.elf` — the linked executable.
- `.build/si7021.map` — the linker map.

If your AVR tools are not on PATH, provide their prefix explicitly:

```sh
make build AVR_PREFIX=/path/to/toolchain/bin/avr-
```

Use forward slashes and avoid spaces in paths passed to Make.

The Makefile only builds the firmware. It does not upload it or operate connected hardware.

## Run on hardware

The steps below are prepared instructions for the current example. They have not yet been verified with a connected sensor.

1. Check your board, sensor module, power supply, and level-shifter specifications.
2. With power disconnected, wire the circuit and check the connections.
3. Build the firmware.
4. Connect the Uno over USB and identify its serial port.
5. Upload the firmware using settings appropriate for its bootloader.

For a classic Uno with the standard Optiboot bootloader:

```sh
avrdude -p atmega328p -c arduino -P YOUR_PORT -b 115200 -D -U flash:w:.build/si7021.hex:i
```

Replace `YOUR_PORT` with the actual serial port. If AVRDUDE cannot locate its matching configuration file, provide it with `-C /path/to/avrdude.conf`.

Open a serial terminal at **9600 baud, 8 data bits, no parity, and 1 stop bit**, then press the Uno's reset button.

The demo is intended to print a banner, one temperature reading, one humidity reading, and a completion message. The values depend on the connected sensor; no measured output from the updated version is included here.

## Clothing-advice example

The earlier application is retained in [`examples/clothing/main.cpp`](examples/clothing/main.cpp). It uses sensor readings and simple thresholds to suggest clothing.

Build it separately:

```powershell
New-Item -ItemType Directory -Force .build/clothing | Out-Null
make build DEMO_SOURCE=examples/clothing/main.cpp BUILD_DIR=.build/clothing
```

It uses three normally open buttons connected between their respective pins and ground:

| Pin | Action |
| --- | --- |
| D10 | Print sensor readings. |
| D12 | Print clothing advice. |
| D11 | Exit the application loop. |

The code enables internal pull-ups. The original thresholds and whole-degree output are preserved.

This example has no button debouncing and samples frequently. It is included as part of the original project, rather than as a polished application. The minimal demo does not require buttons.

## How the library works

The `SI7021` class stores a reference to an externally created `hwlib::i2c_bus`. The caller is responsible for keeping that bus alive while the sensor object uses it.

For each reading, the library:

1. Sends the appropriate no-hold measurement command.
2. Waits for the conversion.
3. Reads two bytes in a single I²C transaction.
4. Combines the bytes and converts the result into a temperature or humidity value.

The implementation uses:

| Operation | Command | Wait |
| --- | --- | --- |
| Software reset | `0xFE` | 15 ms |
| Temperature measurement | `0xF3` | 25 ms |
| Humidity measurement | `0xF5` | 25 ms |

The demo also waits 80 ms before resetting the sensor to allow for power-up.

Temperature and humidity are measured separately. They are not an atomic pair of readings.

The 2026 cleanup corrected command sequencing and changed the result reads to a single two-byte transaction. The public API and conversion formulas were retained. See [`docs/maintenance.md`](docs/maintenance.md) for the technical changes.

## Project structure

| File or directory | Purpose |
| --- | --- |
| `SI7021.hpp` | Public sensor interface. |
| `SI7021.cpp` | Reset, measurement commands, and conversion formulas. |
| `main.cpp` | Minimal temperature and humidity demo. |
| `examples/clothing/main.cpp` | Earlier button-driven clothing-advice application. |
| `Makefile` | AVR build configuration. |
| `tests/` | Host-side tests with simulated hardware primitives. |
| `docs/maintenance.md` | Details of the publication-era fixes and verification. |

## Tests and verification

The host tests require a desktop C++17 compiler and the pinned hwlib dependency.

From PowerShell:

```powershell
New-Item -ItemType Directory -Force .build | Out-Null

g++ -std=c++17 -Wall -Wextra -Werror -Itests -I.deps/hwlib/library/core -I. tests/sensor_test.cpp SI7021.cpp -o .build/sensor_test.exe

.\.build\sensor_test.exe
```

The tests cover:

- Reset-command behaviour.
- Eight conversion and transaction cases.
- Byte ordering.
- ACK/NACK transaction behaviour.
- Decimal formatting, including negative values.

They use the real pinned hwlib I²C transaction classes with simulated GPIO, delays, and sensor responses. They do not measure a physical sensor.

During the October 2026 cleanup:

- The minimal demo compiled and linked with AVR-GCC 11.1.0.
- The clothing-advice example compiled and linked with the same toolchain.
- Both builds produced Intel HEX files.
- Host tests passed with GCC 16.1.0.

No connected hardware was flashed or operated during those checks. Sensor accuracy, actual bus timing, wiring, and serial output remain to be verified on hardware.

## Limitations

- Only the fixed Si7021 address `0x40` is supported.
- CRC validation and reliable bus-error reporting are not implemented.
- The API returns floating-point readings without a separate error status.
- Plausible readings alone do not prove that communication succeeded.
- The underlying hwlib implementation can wait indefinitely if the clock line is stuck.
- Humidity readings are not clamped or calibrated by the library.
- Other boards, including the Uno R4, have not been validated.
- Displaying two decimal places is a formatting choice, not a claim of sensor accuracy.

Before relying on the updated implementation, test it with the actual hardware and compare its readings with a suitable reference.

## About this version

This repository is a cleaned publication of my 2023 project, not a record of continuous development since then.

The 2026 update focused on preserving the original work while making it easier to understand and build. It added a minimal demo, reconstructed the build setup, corrected specific I²C protocol issues, and introduced host tests.

The sensor class, public API, conversion formulas, and earlier clothing-advice application remain recognisable parts of the original project.

## License and acknowledgements

The project retains its existing [Boost Software License 1.0](LICENSE).

It uses [hwlib](https://github.com/wovo/hwlib) by Wouter van Ooijen. The Makefile also retains attribution to his original public-domain template.

The AVR standard-library headers come from [avr-libstdcpp](https://github.com/modm-io/avr-libstdcpp). External dependencies retain their own licenses and notices; consult them when redistributing dependency source or compiled firmware.
