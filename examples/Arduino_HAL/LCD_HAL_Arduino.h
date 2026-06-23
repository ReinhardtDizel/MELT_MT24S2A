/**
 * @file LCD_HAL_Arduino.h
 * @brief HAL реализация для платформы Arduino (пример)
 * @author Рейнгардт Михаил Петрович
 * @version 1.0.0
 * @date 2026
 */

#ifndef LCD_HAL_ARDUINO_H
#define LCD_HAL_ARDUINO_H

#include "LCD_HAL.h"

#if defined(ARDUINO)

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

class LCD_HAL_Arduino : public LCD_HAL {
public:
    LCD_HAL_Arduino() {}
    ~LCD_HAL_Arduino() {}

    void pinModeOutput(uint8_t pin) override {
        pinMode(pin, OUTPUT);
    }
    void pinModeInput(uint8_t pin) override {
        pinMode(pin, INPUT);
    }
    void pinModeInputPullup(uint8_t pin) override {
        pinMode(pin, INPUT_PULLUP);
    }
    void digitalWrite(uint8_t pin, bool value) override {
        ::digitalWrite(pin, value ? HIGH : LOW);
    }
    bool digitalRead(uint8_t pin) override {
        return ::digitalRead(pin) == HIGH;
    }

    void delayMicroseconds(uint32_t us) override {
        ::delayMicroseconds(us);
    }
    void delayMilliseconds(uint32_t ms) override {
        ::delay(ms);
    }

    void i2cInit(uint8_t address) override {
        Wire.begin();
        _i2cAddress = address;
    }
    void i2cWrite(uint8_t data) override {
        Wire.beginTransmission(_i2cAddress);
        Wire.write(data);
        Wire.endTransmission();
    }
    uint8_t i2cRead() override {
        Wire.requestFrom(_i2cAddress, (uint8_t)1);
        if (Wire.available()) return Wire.read();
        return 0;
    }
    void i2cWriteBytes(const uint8_t* data, uint8_t length) override {
        Wire.beginTransmission(_i2cAddress);
        for (uint8_t i = 0; i < length; i++) Wire.write(data[i]);
        Wire.endTransmission();
    }

    void spiInit() override {
        SPI.begin();
    }
    uint8_t spiTransfer(uint8_t data) override {
        return SPI.transfer(data);
    }

    unsigned long millis() override {
        return ::millis();
    }

    const char* getPlatformName() override {
        return "Arduino";
    }
    void init() override {}

private:
    uint8_t _i2cAddress = 0;
};

#endif // defined(ARDUINO)

#endif // LCD_HAL_ARDUINO_H
