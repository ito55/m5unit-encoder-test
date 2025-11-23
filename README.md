# m5unit-encoder-test

This project is based on the example sketch from the [M5Stack Unit Encoder library](https://github.com/m5stack/M5Unit-Encoder). It has been extended with a Serial Debug Interface to allow direct reading and writing of the encoder's I2C registers.

## Serial Debug Interface

You can debug the Unit Encoder by sending commands via the Serial Monitor (Baud Rate: 115200).

### Commands

- **Read Register**: `r,<register_hex>`
  - Example: `r,10` (Reads the value of register 0x10)

- **Write Register**: `w,<register_hex>,<value_hex>`
  - Example: `w,10,ff` (Writes 0xFF to register 0x10)