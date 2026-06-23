/**
 * @file LCD_HAL_Avr.h
 * @brief HAL реализация для AVR (ATmega1284) с прямым доступом к регистрам
 * @author Рейнгардт Михаил Петрович
 * @version 1.0.0
 * @date 2026
 *
 * Нумерация пинов (соответствует вашей схеме):
 *   1-8   → PORTB (бит 0..7)
 *   14-21 → PORTD (бит 0..7)
 *   22-29 → PORTC (бит 0..7)
 * (остальные пины не поддерживаются)
 *
 * Использует собственный таймер (Timer0) для millis().
 */

#ifndef LCD_HAL_AVR_H
#define LCD_HAL_AVR_H

#include "LCD_HAL.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// ---- Собственная реализация millis() на Timer0 ----
static volatile uint32_t _millis_counter = 0;

ISR(TIMER0_COMPA_vect) {
    _millis_counter++;  // каждую 1 мс
}

static void initMillisTimer() {
    // Настройка Timer0 на режим CTC, прерывание по совпадению с OCR0A
    TCCR0A = (1 << WGM01); // CTC mode
    OCR0A = 249;           // для частоты 16 МГц и делителя 64: 16e6/64/250 = 1000 Гц -> 1 мс
    TCCR0B = (1 << CS01) | (1 << CS00); // prescaler 64
    TIMSK0 |= (1 << OCIE0A); // разрешить прерывание по совпадению
    sei();                   // глобально разрешить прерывания
}

static unsigned long getMillis() {
    unsigned long m;
    cli();
    m = _millis_counter;
    sei();
    return m;
}
// ------------------------------------------------

class LCD_HAL_Avr : public LCD_HAL {
public:
    LCD_HAL_Avr() {}
    ~LCD_HAL_Avr() {}

    // ---- GPIO ----
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

    // ---- Задержки ----
    void delayMicroseconds(uint32_t us) override {
        while (us--) _delay_us(1);
    }
    void delayMilliseconds(uint32_t ms) override {
        while (ms--) _delay_ms(1);
    }

    // ---- millis ----
    unsigned long millis() override {
        return getMillis();
    }

    // ---- I2C и SPI (заглушки) ----
    void i2cInit(uint8_t address) override { (void)address; }
    void i2cWrite(uint8_t data) override { (void)data; }
    uint8_t i2cRead() override { return 0; }
    void i2cWriteBytes(const uint8_t* data, uint8_t length) override { (void)data; (void)length; }
    void spiInit() override {}
    uint8_t spiTransfer(uint8_t data) override { (void)data; return 0; }

    // ---- Утилиты ----
    const char* getPlatformName() override {
        return "AVR (ATmega1284)";
    }
    void init() override {
        initMillisTimer();  // запускаем таймер для millis()
    }

private:
    // ---- Маппинг пинов (ваша схема) ----
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
