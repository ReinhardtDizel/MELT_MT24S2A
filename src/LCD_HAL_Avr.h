/**
/**
 * @file LCD_HAL_Avr.h
 * @brief HAL реализация для AVR (прямое управление регистрами)
 * @author Рейнгардт Михаил Петрович
 * @version 1.0.0
 * @date 2026
 */

#ifndef LCD_HAL_AVR_H
#define LCD_HAL_AVR_H

#include "LCD_HAL.h"
#include <avr/io.h>
#include <util/delay.h>

extern unsigned long millis();

/**
 * @class LCD_HAL_Avr
 * @brief Реализация HAL для AVR с прямым доступом к портам
 * 
 * Поддерживает нумерацию пинов:
 *   1-8   → PORTB (бит 0-7)
 *   14-21 → PORTD (бит 0-7)
 *   22-29 → PORTC (бит 0-7)
 * (остальные номера игнорируются)
 */
class LCD_HAL_Avr : public LCD_HAL {
public:
    LCD_HAL_Avr() {}
    ~LCD_HAL_Avr() {}

    // ========================================
    // GPIO с прямым управлением регистрами
    // ========================================

    void pinModeOutput(uint8_t pin) override {
        _setPinMode(pin, true);
    }

    void pinModeInput(uint8_t pin) override {
        _setPinMode(pin, false);
        // Отключаем подтяжку (по умолчанию)
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
            if (value)
                *port |= (1 << bit);
            else
                *port &= ~(1 << bit);
        }
    }

    bool digitalRead(uint8_t pin) override {
        volatile uint8_t* pinReg = _getPin(pin);
        uint8_t bit = _getBit(pin);
        if (pinReg)
            return (*pinReg & (1 << bit)) != 0;
        return false;
    }

    // ========================================
    // Задержки (используем встроенные функции AVR)
    // ========================================

    void delayMicroseconds(uint32_t us) override {
        if (us > 0) _delay_us(us);
    }

    void delayMilliseconds(uint32_t ms) override {
        while (ms--) _delay_ms(1);
    }
    
    unsigned long millis() override {
        return ::millis();
    }

    // ========================================
    // Информация
    // ========================================

    const char* getPlatformName() override {
        return "AVR (direct register)";
    }

    void init() override {
        // Ничего не делаем – всё уже инициализировано
    }

    // ========================================
    // Заглушки для I2C/SPI (если не используются)
    // ========================================

    void i2cInit(uint8_t address) override { (void)address; }
    void i2cWrite(uint8_t data) override { (void)data; }
    uint8_t i2cRead() override { return 0; }
    void i2cWriteBytes(const uint8_t* data, uint8_t length) override { (void)data; (void)length; }
    void spiInit() override {}
    uint8_t spiTransfer(uint8_t data) override { (void)data; return 0; }

private:
    // ========================================
    // Вспомогательные функции для маппинга пинов
    // ========================================

    // Возвращает указатель на регистр DDR для пина (или NULL, если пин недопустим)
    volatile uint8_t* _getDdr(uint8_t pin) {
        if (pin >= 1 && pin <= 8)   return &DDRB;
        if (pin >= 14 && pin <= 21) return &DDRD;
        if (pin >= 22 && pin <= 29) return &DDRC;
        return nullptr;
    }

    // Возвращает указатель на регистр PORT для пина
    volatile uint8_t* _getPort(uint8_t pin) {
        if (pin >= 1 && pin <= 8)   return &PORTB;
        if (pin >= 14 && pin <= 21) return &PORTD;
        if (pin >= 22 && pin <= 29) return &PORTC;
        return nullptr;
    }

    // Возвращает указатель на регистр PIN для пина (чтение)
    volatile uint8_t* _getPin(uint8_t pin) {
        if (pin >= 1 && pin <= 8)   return &PINB;
        if (pin >= 14 && pin <= 21) return &PIND;
        if (pin >= 22 && pin <= 29) return &PINC;
        return nullptr;
    }

    // Возвращает номер бита (0-7)
    uint8_t _getBit(uint8_t pin) {
        if (pin >= 1 && pin <= 8)   return pin - 1;
        if (pin >= 14 && pin <= 21) return pin - 14;
        if (pin >= 22 && pin <= 29) return pin - 22;
        return 0;
    }

    // Установить режим пина (true = выход, false = вход)
    void _setPinMode(uint8_t pin, bool output) {
        volatile uint8_t* ddr = _getDdr(pin);
        uint8_t bit = _getBit(pin);
        if (ddr) {
            if (output)
                *ddr |= (1 << bit);
            else
                *ddr &= ~(1 << bit);
        }
    }

    // Включить/отключить подтяжку (работает только в режиме входа)
    void _setPinPullup(uint8_t pin, bool enable) {
        volatile uint8_t* port = _getPort(pin);
        uint8_t bit = _getBit(pin);
        if (port) {
            if (enable)
                *port |= (1 << bit);
            else
                *port &= ~(1 << bit);
        }
    }
};

#endif // LCD_HAL_AVR_H