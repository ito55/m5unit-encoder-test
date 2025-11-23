# m5unit-encoder-test

This project is based on the example sketch from the [M5Stack Unit Encoder library](https://github.com/m5stack/M5Unit-Encoder). It has been extended with a Serial Debug Interface to allow direct reading and writing of the encoder's I2C registers.

## Serial Debug Interface

You can debug the Unit Encoder by sending commands via the Serial Monitor (Baud Rate: 115200).

### Commands

- **Read Register**: `r,<register_hex>`
  - Example: `r,10` (Reads the value of register 0x10)

- **Write Register**: `w,<register_hex>,<value_hex>`
  - Example: `w,10,ff` (Writes 0xFF to register 0x10)

- **Write Multiple Bytes**: `W,<register_hex>,<val1_hex>,<val2_hex>,...`
  - Example: `W,30,00,ff,00,00` (Writes 4 bytes starting at register 0x30)

- **Toggle Debug Mode**: `M`
  - Toggles the `led_debug_mode`. When ON, the automatic LED control in the loop is disabled, allowing manual LED control via serial commands.

## Firmware Limitations

Based on testing, some Unit Encoder devices (likely Firmware v1.1 or earlier) have the following behavior:

- **LED Control**: Works correctly using the `W` command to write to register `0x30`.
- **Encoder Reset**: Writing to the Encoder Value register (`0x10`) or the Mode register (`0x00`) **does not work**. The value cannot be reset or overwritten via I2C.
- **Firmware Version Check**: Reading register `0xFE` returns `0x01`.

If you encounter this behavior, your device likely does not support I2C-based counter reset.