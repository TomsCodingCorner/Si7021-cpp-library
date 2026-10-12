# Publication maintenance record

This records the changes made on 11 October 2026. It is not an account of
the author's original motivation or a claim of ongoing maintenance.

## Preserved implementation

The `SI7021` class still owns no bus and stores the supplied bus reference.
Constructor and method signatures are unchanged. The two original floating
point conversion equations, raw-byte assembly, fixed address `0x40`, 15 ms
software-reset delay and 25 ms measurement delays are retained. Humidity is
not clamped or calibrated by this library. The clothing example retains its
original thresholds, button assignments and integer-degree truncation.

## Protocol repairs

The manufacturer's [Si7021-A20 datasheet, sections 5–5.1](https://cdn-learn.adafruit.com/assets/assets/000/035/931/original/Support_Documents_TechnicalDocs_Si7021-A20.pdf)
and the [pinned hwlib transaction implementation](https://github.com/wovo/hwlib/blob/d733d8aca4e1fdbd09f2cbb2a0b2390eeb583dcf/library/core/hwlib-i2c.hpp)
are the basis for these functional changes:

| Original behavior | Publication behavior | Reason |
| --- | --- | --- |
| Temperature command `0xE3`, followed by separate writes of `0xE0 >> 8` and `0xE0 & 0xFF` | One no-hold temperature command `0xF3`, wait, read | The shifted byte was `0x00`, an undefined command. `0xE0` reads temperature from a previous RH conversion, rather than the independently requested temperature. |
| Humidity command `0xF5`, wait, another command `0xE5`, then wait | One `0xF5`, wait, read | The second command initiated another conversion. A single no-hold command matches the existing fixed-wait approach. |
| Each result byte read through a separate temporary `bus.read(...)` | `bus.read(0x40).read(data, 2)` | Both bytes must belong to one transfer. hwlib sends ACK between them, then NACK and STOP on transaction destruction. |
| Extra 10 ms waits after the removed commands | Removed with those commands | The retained 25 ms wait follows the one actual conversion request. |

No register configuration or new sensor feature was added. Optional CRC and
ACK-error reporting remain absent; changing that would require a separate
design decision about the API and bus abstraction. These repairs have host
verification, not physical validation.

## Examples and build

The original `testcode.cpp` became root `main.cpp`, so the default build now
demonstrates reset and one temperature/humidity read without buttons. The
original multi-character unit literal and integer-only presentation were
replaced by a unit string and explicit hundredths formatting. Printing two
decimals is formatting, not a claim of measurement accuracy. Added startup
delay and a sanity message are documented in the source.

Original root `main.cpp` became `examples/clothing/main.cpp`. Helper/button
names were made descriptive, an informal comment was replaced, excess blank
lines were removed and a desktop-window exit instruction was removed from
the embedded output. No clothing thresholds were changed.

The original Makefile delegated to a missing `../Makefile.uno`; neither
that wrapper nor a hwlib version was supplied. The publication Makefile
provides an explicit Uno AVR build using pinned dependencies. This is
current build reconstruction, not proof of the original toolchain version.
The author/vendor attribution and existing project license are retained.

## Actual verification

- Host: GCC 16.1.0, C++17, `-Wall -Wextra -Werror`; reset trace, eight
  conversion/transaction cases and formatting passed. Real upstream hwlib
  transaction classes were used with fake primitives and delays.
- AVR: minimal demo compiled and linked successfully with AVR-GCC 11.1.0,
  C++17 and the pinned hwlib/AVR standard headers listed in the README.
  Intel HEX generation succeeded. `avr-size` reports 5396 bytes text,
  424 bytes data and 17 bytes BSS (5820 bytes of flash content and
  441 bytes of static RAM, excluding stack). Upstream hwlib reports unused
  parameter and unsigned-comparison warnings; dependency code was not edited.
- AVR: the preserved clothing example also compiled, linked and generated
  Intel HEX with the same dependencies. It reports 6010 bytes text,
  782 bytes data and 17 bytes BSS (6792 bytes flash, 799 bytes static RAM).
- Initial Arduino AVR-GCC 7.3.0 build failed because standard C++ headers
  were absent. Adding the selected AVR headers to that compiler exposed
  unsupported compiler intrinsics. Use the documented compiler combination.
- Original source material is preserved in verified private backups and
  the existing history; backup hashes are held outside this source tree.
- No real sensor, supply voltage, bus timing, logic-level circuit, serial
  measurement output or upload was tested. No pre-existing automated tests
  were supplied: the old test program was a hardware example.

## Material outside the publication

The original planning/reflection documents and school poster/presentation
remain in private backups and the existing history. Personal content and stock/template imagery
make them unsuitable for automatic republication. No manufacturer datasheet
is bundled. The separate extensionless Arduino Wire example is excluded:
its provenance is unclear, its empty conditional statements undermine its
byte-count checks, and it does not call `SI7021`.

Private review notes, toolchain downloads, logs and originals are outside
the publication tree. Do not import the old Git history into a new public
repository; the cleaned source tree is the publication boundary.

## Integration into the existing repository

On 12 October 2026, the author requested these improvements in the existing
GitHub repository. The eleven cleaned source files were already present
locally and matched the reviewed publication version. Four old PDFs, the
extensionless Wire sketch and the replaced `testcode.cpp` were removed from
the current tree. Both the previous working files and original HEAD were
backed up outside the repository before those removals.

This integration retains the existing history and does not change repository
visibility. The removed material remains accessible in earlier commits.
Use a fresh repository from the cleaned files for a public version without
those originals. No hardware was operated as part of this integration.
The host tests and minimal AVR firmware build were rerun successfully from
the current repository. All generated output was kept outside the repository.
