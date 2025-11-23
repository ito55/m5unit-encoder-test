#include <M5Unified.h>
#include <Unit_Encoder.h>

M5Canvas canvas(&M5.Display);
Unit_Encoder sensor;

void setup() {
    auto cfg = M5.config();
    cfg.clear_display = true;
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);
    int ex_sda = M5.getPin(m5::ex_i2c_sda);
    int ex_scl = M5.getPin(m5::ex_i2c_scl);
    if (ex_sda >= 0 && ex_scl >= 0) {
        sensor.begin(&Wire, ENCODER_ADDR, ex_sda, ex_scl); // I2C address for encoder: 0x40 (ENCODER_ADDR defined in Unit_Encoder.h)
    } else {
        sensor.begin(&Wire);
    }
    canvas.createSprite(M5.Display.width(), 80);
    canvas.setTextSize(2);
}

signed short int last_value = 0;

void writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(ENCODER_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
    Serial.printf("Write: Reg 0x%02X, Val 0x%02X\n", reg, value);
}

uint8_t readRegister(uint8_t reg) {
    Wire.beginTransmission(ENCODER_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((int)ENCODER_ADDR, 1);
    if (Wire.available()) {
        uint8_t val = Wire.read();
        Serial.printf("Read: Reg 0x%02X, Val 0x%02X\n", reg, val);
        return val;
    }
    Serial.printf("Read: Reg 0x%02X Failed\n", reg);
    return 0;
}

bool led_debug_mode = false;

void processSerialCommands() {
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        if (cmd.length() == 0) return;

        if (cmd.startsWith("r,")) {
            // r,10
            int reg = strtol(cmd.substring(2).c_str(), NULL, 16);
            readRegister((uint8_t)reg);
        } else if (cmd.startsWith("w,")) {
            // w,10,ff
            int firstComma = cmd.indexOf(',');
            int secondComma = cmd.indexOf(',', firstComma + 1);
            if (secondComma != -1) {
                int reg = strtol(cmd.substring(firstComma + 1, secondComma).c_str(), NULL, 16);
                int val = strtol(cmd.substring(secondComma + 1).c_str(), NULL, 16);
                writeRegister((uint8_t)reg, (uint8_t)val);
            }
        } else if (cmd.startsWith("W,")) {
            // W,30,00,ff,00,00
            int firstComma = cmd.indexOf(',');
            int reg = strtol(cmd.substring(firstComma + 1).c_str(), NULL, 16);
            
            Wire.beginTransmission(ENCODER_ADDR);
            Wire.write((uint8_t)reg);
            
            int currentComma = firstComma;
            int nextComma = cmd.indexOf(',', currentComma + 1);
            
            Serial.printf("WriteMulti: Reg 0x%02X", reg);
            
            while (nextComma != -1) {
                int valComma = cmd.indexOf(',', nextComma + 1);
                String valStr;
                if (valComma == -1) {
                    valStr = cmd.substring(nextComma + 1);
                } else {
                    valStr = cmd.substring(nextComma + 1, valComma);
                }
                int val = strtol(valStr.c_str(), NULL, 16);
                Wire.write((uint8_t)val);
                Serial.printf(", 0x%02X", val);
                
                nextComma = valComma;
            }
            Wire.endTransmission();
            Serial.println();
        } else if (cmd.equalsIgnoreCase("M")) {
            led_debug_mode = !led_debug_mode;
            Serial.printf("Debug Mode: %s\n", led_debug_mode ? "ON" : "OFF");
        }
    }
}

void loop() {
    processSerialCommands();
    signed short int encoder_value = sensor.getEncoderValue();
    bool btn_status                = sensor.getButtonStatus();
    if (last_value != encoder_value) {
        Serial.println(encoder_value);
        if (!led_debug_mode) {
            if (last_value > encoder_value) {
                sensor.setLEDColor(1, 0x000011);
            } else {
                sensor.setLEDColor(2, 0x111100);
            }
        }
        last_value = encoder_value;
    } else {
        if (!led_debug_mode) {
            sensor.setLEDColor(0, 0x001100);
        }
    }
    if (!btn_status) {
        if (!led_debug_mode) {
            sensor.setLEDColor(0, 0xC800FF);
        }
    }
    canvas.fillSprite(BLACK);
    canvas.setCursor(0, 0);
    canvas.print("BTN: ");
    canvas.println(btn_status);
    canvas.print("ENC: ");
    canvas.println(encoder_value);
    canvas.pushSprite(0, 0);
    delay(20);
}