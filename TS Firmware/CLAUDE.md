# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is firmware for a TubeScreamer guitar effects module built on a Raspberry Pi Pico (RP2040) using the Arduino framework. The module features:
- 3 continuous rotation potentiometers with dual ADC channels for position tracking
- 2 three-position toggle switches
- WS2812B LED rings (16 LEDs per pot, 54 total LEDs) providing visual feedback
- Hardware interface using SPI-based MCP3008 ADC and custom libraries

## Build System

This project uses PlatformIO for building and uploading firmware.

### Common Commands

**Build the project:**
```bash
pio run
```

**Upload to device:**
```bash
pio run --target upload
```

**Clean build artifacts:**
```bash
pio run --target clean
```

**Monitor serial output:**
```bash
pio device monitor
```

**Build and upload in one command:**
```bash
pio run --target upload && pio device monitor
```

### Build Configuration

Target board: Raspberry Pi Pico (RP2040) using Earlephilhower Arduino core
- Platform: Custom RP2040 platform from maxgerhardt/platform-raspberrypi
- Upload protocol: picotool

## Architecture

### Hardware Abstraction Layer

**[hardware_def.h](include/hardware_def.h)** - Central hardware configuration
- Pin definitions for all peripherals (LEDs, ADC SPI, toggles)
- ADC channel mappings for the 3 dual-channel continuous pots
- LED indexing constants for pot rings and toggle indicator LEDs

### Core Components

**[main.cpp](src/main.cpp)** - Entry point and main control loop
- Initializes interface on startup
- Main loop: reads pots, updates LED bars, handles toggle switch events
- 10ms cycle time for responsive control

**[interface.cpp](src/interface.cpp) / [interface.h](src/interface.h)** - Hardware interface layer
- Manages all I/O: continuous pots (via MCP3008 ADC), toggle switches, LED rings
- `initInterface()`: Sets up FastLED, SPI for ADC, interrupt handlers for toggles
- `readPots()`: Reads all 6 ADC channels (2 per pot), processes continuous rotation tracking, applies wrap-around logic with `POT_WRAP_THRESHOLD`
- Toggle switch handlers use interrupt-driven flags for debounced state detection

### External Libraries (Custom)

The project depends on several custom libraries hosted on GitHub:
- **LED-Bar**: Manages LED ring displays with gradient fills and multiple modes (centered, fill, wrap)
- **Continuous-Pots**: Processes dual-channel quadrature-like input from continuous rotation potentiometers
- **mcp3008**: SPI interface for MCP3008 8-channel 10-bit ADC
- **digipot**: Digital potentiometer control (not currently used in code)

These libraries are included via PlatformIO's `lib_deps` with `lib_ldf_mode = deep` for dependency resolution.

## Key Design Patterns

### Continuous Pot Processing
Each continuous pot uses two ADC channels to determine rotation direction and position. The firmware:
1. Reads both channels per pot (6 total ADC reads)
2. Uses `contPot_update()` to calculate absolute position and direction
3. Applies delta to LED bar value with boundary clamping (0-1023)
4. Implements wrap detection via `POT_WRAP_THRESHOLD` to prevent jumps when crossing zero

### LED Bar Feedback
LED rings provide visual feedback for pot positions:
- Each pot has a 16-LED ring mapped via `firstLedIndex`
- Supports multiple modes: `PotCentred`, `PotFill`
- Gradient rendering between two colors (e.g., neon pink to light blue)
- Updated only when pot values change to reduce FastLED.show() calls

### Toggle Switch Handling
3-position toggles use two pins each to encode position (left/middle/right):
- Pin state combinations: (HIGH, LOW) = left, (LOW, HIGH) = right, (HIGH, HIGH) = middle
- Interrupt-driven detection with flag-based deferred handling in main loop
- 20ms software debounce delay in handlers
- Individual LEDs illuminate to indicate toggle position

## Development Notes

### Serial Debugging
Build flag `-D SERIAL_PRINT_POTS` enables pot value logging to Serial at 115200 baud. Pot values and directions print when changes are detected.

### Hardware SPI Configuration
The MCP3008 ADC uses SPI1 with custom pin assignments:
- SCK: GPIO 26, MISO: GPIO 24, MOSI: GPIO 27, CS: GPIO 25
- Configured via `ADC_SPI.setSCK/setRX/setTX()` before `begin()`

### LED Data Direction
LED rings can be configured with `dataDirection = 1` (clockwise) or `-1` (counter-clockwise) to match physical wiring orientation.
