# Si7021 temperature and humidity library

A small C++ library for reading a Silicon Labs Si7021 over I2C using
[hwlib](https://github.com/wovo/hwlib). It exposes sensor reset, temperature
in degrees Celsius and relative humidity in percent. The example targets
the classic Arduino Uno / ATmega328P at 16 MHz.

This is a cleaned publication of an early hardware project, originally a
school assignment that developed into a personal project with a self-written
sensor library. The supplied planning and reflection documents describe
writing the Si7021 library and a clothing-advice application. They also
describe switching from a BME280 idea to the Si7021; BME280 support is not
implemented here.

This is a project dated closed on **July 2023** and prepared for showcasing on **October 2026**. The current
repository's 2026 import commit does not establish the original timeline.

## What is included

| File | Role and provenance |
| --- | --- |
| `SI7021.hpp`, `SI7021.cpp` | Project-specific sensor class, consistent with the author's description of a self-written library. Public API and conversion formulas preserved; I2C repairs documented below. |
| `main.cpp` | Minimal demo adapted from the original `testcode.cpp`. |
| `examples/clothing/main.cpp` | Earlier button-driven application, with descriptive names and small comment/output cleanup. Original thresholds and whole-degree truncation retained. |
| `Makefile` | Current build recipe; retains the attribution to Wouter van Ooijen's original public-domain template. |
| `tests/` | Publication-era host tests with fake hardware primitives and real upstream hwlib transaction classes. |
| `LICENSE` | Existing Boost Software License, retained unchanged. No new license was selected. |

The library keeps a reference to an externally constructed `hwlib::i2c_bus`.
The demo uses open-collector GPIO pins and `i2c_bus_bit_banged_scl_sda`;
the sensor traffic is software I2C, rather than Arduino `Wire`. Console
output uses hwlib's UART support. Neither hwlib nor the compiler is the
project author's implementation.

Implemented: `reset()`, `readTemperature()` and `readHumidity()` at fixed
7-bit address `0x40`. There is no heater control, resolution selection,
device identification, CRC validation or reliable bus-error reporting.
Temperature and humidity are separate measurements, not an atomic pair.

## Hardware and wiring

The Makefile identifies an Uno target; the actual original board revision
and sensor breakout manufacturer are not recorded. The old illustrations
show a generic purple board marked for multiple sensor types; they do not
identify the installed chip, regulator or level shifting. Verify the actual
module before powering it. Uno R4 and other boards are not validated targets.

Use a classic Uno, USB cable, a confirmed Si7021 module, breadboard/jumpers,
and suitable I2C level shifting. This table is a **proposed Uno demo setup**,
not a recovered or hardware-tested original wiring diagram.

| Uno connection | Sensor connection | Condition |
| --- | --- | --- |
| SDA / A4 | SDA through bidirectional I2C level shifter | High side at Uno logic supply; low side at sensor logic supply. |
| SCL / A5 | SCL through bidirectional I2C level shifter | hwlib's `pins::scl` maps to A5; `pins::sda` maps to A4. |
| GND | GND and level-shifter ground | Common ground. |
| Regulated 3.3 V | Bare-chip VDD or documented 3.3 V module input | Confirm the module's power path and labels first. |

The Si7021-A20 chip requires 1.9–3.6 V; SDA/SCL operating inputs stay within
0–VDD. Its typical circuit uses two 10 kOhm pull-ups to VDD and 0.1 uF
decoupling. A module or level shifter may already include these parts.
Avoid duplicate pull-ups without checking the resulting resistance. A
5 V Uno needs compatible logic translation unless a documented module
already provides it. A regulator alone does not prove 5 V I/O tolerance.
The chip has no external reset/enable pin; reset is command `0xFE`.

These electrical limits come from the **Silicon Labs Si7021-A20 datasheet**,
Tables 1–3 and Figure 4, available as a
[manufacturer-authored copy hosted by Adafruit](https://cdn-learn.adafruit.com/assets/assets/000/035/931/original/Support_Documents_TechnicalDocs_Si7021-A20.pdf).
The [original manufacturer URL](https://www.silabs.com/documents/public/data-sheets/Si7021-A20.pdf)
redirected to the sensor overview during this cleanup. The project's exact
package suffix/revision remains unconfirmed. See also the
[official Uno R3 documentation](https://docs.arduino.cc/hardware/uno-rev3/) and
[checked hwlib Uno mapping](https://github.com/wovo/hwlib/blob/d733d8aca4e1fdbd09f2cbb2a0b2390eeb583dcf/library/targets/hwlib-arduino-uno.hpp).

The library retains 15 ms after reset and 25 ms after each no-hold
measurement (`0xF3` temperature, `0xF5` humidity). The demo adds 80 ms
before reset for power-up. Datasheet Table 2 supports these waits, including
the RH measurement's additional temperature conversion. Each read receives
two bytes in one transaction, ACKs the first and NACKs the last; optional CRC
is omitted. See datasheet sections 5–5.1. The bit-banged clock is controlled
by hwlib and instruction overhead; no measured frequency is available. Check
it on hardware against the datasheet's 400 kHz maximum.

## Dependencies and build

The original build included `../Makefile.uno`, which was not supplied.
The current recipe reconstructs an explicit AVR build and does not claim to
reproduce the exact school toolchain. It requires GNU Make, Git and AVR-GCC
with C++17 support. Arduino IDE / `Wire` is not the build environment for
this library.

Use these pinned source dependencies (download locally; do not publish them
as part of this project):

```sh
git clone https://github.com/wovo/hwlib.git .deps/hwlib
git -C .deps/hwlib checkout d733d8aca4e1fdbd09f2cbb2a0b2390eeb583dcf
git clone https://github.com/modm-io/avr-libstdcpp.git .deps/avr-libstdcpp
git -C .deps/avr-libstdcpp checkout 9d10e0047011f4c1e6ef4dbf76aca5e9a340a6f8
```

For the verified compiler combination, obtain **AVR-GCC 11.1.0**, release
[`v11.1.0-1` from its toolchain maintainer](https://github.com/ZakKemble/avr-gcc-build/releases/tag/v11.1.0-1).
Choose the archive for your operating system, verify it against the release's
`SHA256SUMS`, extract it and put its `bin` directory on PATH. The additional
`avr-libstdcpp` headers are a current build dependency needed by hwlib; their
use in the original school environment is not established. Do not substitute
Arduino's AVR-GCC 7.3.0 with these headers: that combination failed here.

From this directory, in PowerShell:

```powershell
New-Item -ItemType Directory -Force .build | Out-Null
avr-g++ --version
make build
```

On POSIX shells replace the directory command with `mkdir -p .build`.
On Windows, use a working GNU Make installation/shell, such as MSYS2.
Paths passed as Make variables should use forward slashes and avoid spaces.
If the AVR tools are outside PATH, pass
`make build AVR_PREFIX=/path/to/toolchain/bin/avr-`.
The output is `.build/si7021.hex`, with ELF/map files alongside it. No Make
target flashes or runs hardware. No Arduino core is linked.

The older application can be built separately:

```powershell
New-Item -ItemType Directory -Force .build/clothing | Out-Null
make build DEMO_SOURCE=examples/clothing/main.cpp BUILD_DIR=.build/clothing
```

It needs three normally open buttons to ground: D10 prints readings, D12
prints clothing advice, D11 exits the loop. The code enables internal
pull-ups. It samples frequently, has no debounce, and intentionally retains
the earlier whole-degree output. The minimal demo needs no buttons.

## Run the minimal demo

1. Confirm the board, chip/module and level-shifter specifications. Disconnect
   power, wire the table above, then check continuity, polarity and pull-ups.
2. Build the firmware. Connect the Uno by USB and identify its actual serial
   port. Verify the bootloader before choosing upload settings.
3. For a **classic Uno with its standard Optiboot bootloader**, manually run:

   ```sh
   avrdude -p atmega328p -c arduino -P YOUR_PORT -b 115200 -D -U flash:w:.build/si7021.hex:i
   ```

   Replace `YOUR_PORT` with the actual port (for example a confirmed COM
   port). Use an AVRDUDE installation with its matching configuration file;
   if it does not find that file, supply `-C /path/to/avrdude.conf`. This
   command is a prepared instruction; it has not been executed here.
4. Open a serial terminal at **9600 baud, 8 data bits, no parity, 1 stop bit**,
   then press the Uno's reset button. Expect a banner, one temperature line,
   one humidity line and a completion message. The numbers depend on your
   sensor; no measured output is included in this publication.

The API used by the actual demo is:

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

The demo's unexpected-reading message is only a range check. A plausible
number does not prove an acknowledged transfer or a correct sensor. The
unchanged API returns floats and exposes no bus status. hwlib also discards
ACK results and can wait indefinitely on a stuck clock line.

## Verification and remaining limits

Host checks can be reproduced with a desktop C++17 compiler after fetching
hwlib:

```powershell
New-Item -ItemType Directory -Force .build | Out-Null
g++ -std=c++17 -Wall -Wextra -Werror -Itests -I.deps/hwlib/library/core -I. tests/sensor_test.cpp SI7021.cpp -o .build/sensor_test.exe
./.build/sensor_test.exe
```

The tests check reset, eight input conversions and their transaction traces,
byte order, ACK/NACK and decimal formatting including negative values.
They replace GPIO, delays and sensor responses; they use the real pinned
hwlib I2C transaction implementation. They are not sensor measurements.

Publication verification was performed on 11 October 2026: the minimal demo
and the retained clothing example compiled and linked with AVR-GCC 11.1.0
and produced Intel HEX files;
the host tests passed with GCC 16.1.0. See
[maintenance notes](docs/maintenance.md) for the exact build results and the
changes to the original protocol. No connected hardware was operated or
flashed; no new physical demonstration was performed. No recorded measured
results or identifiable original circuit photo were supplied.

Before a physical demonstration, confirm the actual chip and module, measure
both bus-side voltages, check the clock and transaction trace with a logic
analyzer if available, and compare readings with a reference instrument.
Record tool versions, wiring and real output only after performing those
checks. A successful build or host test proves neither sensor accuracy nor
electrical compatibility.

## Publication cleanup

The class, bus abstraction, conversion equations, reset delay and overall
approach remain recognizable. The public API is unchanged. The minimal demo,
build instructions and host tests are current maintenance additions.
Specific protocol repairs are explicitly separated in
[the maintenance notes](docs/maintenance.md).

Personal school documents and poster/presentation assets are retained in
private backups and the existing history, outside the current source tree. Their imagery has
unclear redistribution rights and is not evidence of the actual circuit.
The separate Arduino Wire sketch is also omitted because its authorship is
not established and it does not exercise this library. Dependencies keep
their upstream notices: hwlib uses Boost Software License 1.0; the AVR
standard headers carry their own GPL/Runtime Library Exception notices.
Consult those upstream licenses if redistributing dependency source or
firmware. The existing project `LICENSE` is preserved as found.

At the author's request, the cleaned files are now integrated into the
existing repository. The six excluded legacy files were removed from its
current source tree after a verified private backup. **The existing Git
history still contains the original files.** This update does not sanitize
that history or change repository visibility.

For public portfolio publication, use only this cleaned source tree,
excluding `.deps`, `.build`, private review notes, original PDFs and the old
`.git` directory. Create a **new repository with fresh history**. If using a
template, use a template made from the cleaned files and leave **Include all
branches** disabled. Changing files or choosing that checkbox on the old
repository does not itself sanitize its content. No old history has been
rewritten and no commits have been backdated.
