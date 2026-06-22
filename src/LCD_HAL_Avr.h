#ifndef LCD_HAL_AVR_H
#define LCD_HAL_AVR_H

#include "LCD_HAL.h"
#include <avr/io.h>
#include <util/delay.h>

class LCD_HAL_Avr : public LCD_HAL {
public:
    LCD_HAL_Avr() {}
    ~LCD_HAL_Avr() {}

    void pinModeOutput(uint8_t pin) override {
        _setPinMode(pin, true);
    }
    void pinModeInput(uint8_t pin) override {
        _setPinMode(pin, false);
        _setPinPullup(pin, false);
    }
    void pinModeInputPullup(uint8_t pin) override {
        _setPinMode(pin, false);
        _setPinPullup(pin, true);
    }
    void digitalWrite(uint8_t pin, bool value) override {
        volatile uint8_t* port = _getPort(pin);
        uint8_t bit = _getBit(pin);
        if (port) {
            if (value) *port |= (1 << bit);
            else       *port &= ~(1 << bit);
        }
    }
    bool digitalRead(uint8_t pin) override {
        volatile uint8_t* pinReg = _getPin(pin);
        uint8_t bit = _getBit(pin);
        if (pinReg) return (*pinReg & (1 << bit)) != 0;
        return false;
    }

    void delayMicroseconds(uint32_t us) override {
        // Исправлено: цикл вместо прямого вызова _delay_us(us)
        while (us--) _delay_us(1);
    }

    void delayMilliseconds(uint32_t ms) override {
        while (ms--) _delay_ms(1);
    }

    unsigned long millis() override {
        return ::millis();
    }

    const char* getPlatformName() override {
        return "AVR (direct register)";
    }
    void init() override {}

    void i2cInit(uint8_t address) override { (void)address; }
    void i2cWrite(uint8_t data) override { (void)data; }
    uint8_t i2cRead() override { return 0; }
    void i2cWriteBytes(const uint8_t* data, uint8_t length) override { (void)data; (void)length; }
    void spiInit() override {}
    uint8_t spiTransfer(uint8_t data) override { (void)data; return 0; }

private:
    volatile uint8_t* _getDdr(uint8_t pin) {
        if (pin >= 1 && pin <= 8)   return &DDRB;
        if (pin >= 14 && pin <= 21) return &DDRD;
        if (pin >= 22 && pin <= 29) return &DDRC;
        return nullptr;
    }
    volatile uint8_t* _getPort(uint8_t pin) {
        if (pin >= 1 && pin <= 8)   return &PORTB;
        if (pin >= 14 && pin <= 21) return &PORTD;
        if (pin >= 22 && pin <= 29) return &PORTC;
        return nullptr;
    }
    volatile uint8_t* _getPin(uint8_t pin) {
        if (pin >= 1 && pin <= 8)   return &PINB;
        if (pin >= 14 && pin <= 21) return &PIND;
        if (pin >= 22 && pin <= 29) return &PINC;
        return nullptr;
    }
    uint8_t _getBit(uint8_t pin) {
        if (pin >= 1 && pin <= 8)   return pin - 1;
        if (pin >= 14 && pin <= 21) return pin - 14;
        if (pin >= 22 && pin <= 29) return pin - 22;
        return 0;
    }
    void _setPinMode(uint8_t pin, bool output) {
        volatile uint8_t* ddr = _getDdr(pin);
        uint8_t bit = _getBit(pin);
        if (ddr) {
            if (output) *ddr |= (1 << bit);
            else        *ddr &= ~(1 << bit);
        }
    }
    void _setPinPullup(uint8_t pin, bool enable) {
        volatile uint8_t* port = _getPort(pin);
        uint8_t bit = _getBit(pin);
        if (port) {
            if (enable) *port |= (1 << bit);
            else        *port &= ~(1 << bit);
        }
    }
};

#endif // LCD_HAL_AVR_H