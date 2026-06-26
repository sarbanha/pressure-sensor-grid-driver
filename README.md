# Pressure Sensor Grid Driver (PSGD)

A real-time I2C-based pressure sensor grid driver for Arduino microcontrollers, reading a 4x4 array of MMR920 pressure sensors and streaming calibrated data over serial.

## Overview

This firmware manages a 16-sensor pressure grid using I2C communication. It continuously acquires pressure measurements at 401 µs conversion intervals and outputs data in CSV format for real-time acquisition and MATLAB integration.

## Hardware

- **Microcontroller**: Arduino-compatible board with I2C support (SDA/SCL)
- **Sensors**: 16× MMR920 pressure transducers (4×4 grid)
- **I2C Addresses**: 0x00–0x0F (configurable in `sensor_addr[]`)
- **Input**: Digital switch on pin 21 for event logging
- **Communication**: 1 Mbps serial (UART)

## Features

- **16-channel simultaneous sampling**: Independent I2C channels for all sensors
- **Calibrated output**: Factory-calibrated pressure values in bar
- **Low latency**: 401 µs sensor conversion time + minimal controller overhead
- **CSV streaming**: Real-time data output: `Timestamp,Address,Pressure[bar],SwitchFlag`
- **Error handling**: I2C timeout and transmission error detection

## Quick Start

1. Upload `psgd.ino` to your microcontroller
2. Connect sensors to I2C bus (SCL pin 22, SDA pin 21 on typical boards)
3. Open serial monitor at **1,000,000 baud**
4. Watch for `READY` confirmation, followed by CSV data stream

## Data Output Format

```
Micros,Addr,Pressure[bar],Flag
12345,0x00,2.15430,0
12746,0x01,2.14521,1
...
```

- **Micros**: Microsecond timestamp from `micros()`
- **Addr**: Sensor I2C address (hex)
- **Pressure[bar]**: Calibrated pressure reading
- **Flag**: Digital switch state (0 or 1)

## Calibration

Pressure conversion includes empirical calibration coefficients:
- Formula: `pressure_bar = (raw_value / 100000.0) × 0.554 + 0.2273`
- Adjust coefficients in the `loop()` function if using different sensor variants

## I2C Protocol

- **Bus speed**: 400 kHz
- **Timeout**: 10 ms per transaction
- **Commands**:
  - `0x72` – Reset sensor
  - `0xA0` – Start ACTIVE1 measurement mode
  - `0xC0` – Read pressure result

## License

See [LICENSE](LICENSE)
