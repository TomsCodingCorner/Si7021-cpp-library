#############################################################################
# Project Makefile
# Original template (c) Wouter van Ooijen (www.voti.nl) 2016.
# The original template is in the public domain.
# Publication maintenance: explicit AVR build instead of missing Makefile.uno.
#############################################################################

.DEFAULT_GOAL := build
HWLIB ?= .deps/hwlib
AVR_STDLIB ?= .deps/avr-libstdcpp
AVR_PREFIX ?= avr-
BUILD_DIR ?= .build
DEMO_SOURCE ?= main.cpp

CXX := $(AVR_PREFIX)g++
OBJCOPY := $(AVR_PREFIX)objcopy
SIZE := $(AVR_PREFIX)size
CPPFLAGS := -I. -isystem $(HWLIB)/library -isystem $(AVR_STDLIB)/include -DHWLIB_TARGET_arduino_uno -DF_CPU=16000000UL -DBMPTK_BAUDRATE=9600
CXXFLAGS := -mmcu=atmega328p -std=c++17 -Os -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -Wall -Wextra
LDFLAGS := -mmcu=atmega328p -Wl,--gc-sections,-Map,$(BUILD_DIR)/si7021.map

.PHONY: build check-dependencies
build: $(BUILD_DIR)/si7021.hex

check-dependencies:
	$(if $(wildcard $(HWLIB)/library/hwlib.hpp),,$(error Missing hwlib. See README dependency setup))
	$(if $(wildcard $(AVR_STDLIB)/include/cstdint),,$(error Missing AVR C++ headers. See README dependency setup))

$(BUILD_DIR)/si7021.elf: $(DEMO_SOURCE) SI7021.cpp SI7021.hpp $(wildcard $(HWLIB)/library/*.hpp $(HWLIB)/library/*/*.hpp) | check-dependencies
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEMO_SOURCE) SI7021.cpp $(HWLIB)/library/hwlib.cpp $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD_DIR)/si7021.hex: $(BUILD_DIR)/si7021.elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Create BUILD_DIR before building (see README). No target uploads hardware.
