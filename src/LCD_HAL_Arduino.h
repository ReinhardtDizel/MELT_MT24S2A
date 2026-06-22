/**
 * @file LCD_HAL_Arduino.h
 * @brief HAL реализация для платформы Arduino
 * @author Рейнгардт Михаил Петрович
 * @version 1.0.0
 * @date 2026
 */

#ifndef LCD_HAL_ARDUINO_H
#define LCD_HAL_ARDUINO_H

#include "LCD_HAL.h"

// Проверка, что компилируем для Arduino
#if defined(ARDUINO)

#include <Arduino.h>

/**
 * @class LCD_HAL_Arduino
 * @brief Реализация HAL для платформ Arduino (Uno, Nano, Mega и т.д.)
 */
class LCD_HAL_Arduino : public LCD_HAL {
public:
    /**
     * @brief Конструктор
     */
    LCD_HAL_Arduino() {}
    
    /**
     * @brief Деструктор
     */
    ~LCD_HAL_Arduino() {}
    
    // ========================================
    // Реализация методов работы с GPIO
    // ========================================
    
    /**
     * @brief Настройка пина на выход
     * @param pin Номер пина
     */
    void pinModeOutput(uint8_t pin) override {
        pinMode(pin, OUTPUT);
    }
    
    /**
     * @brief Настройка пина на вход
     * @param pin Номер пина
     */
    void pinModeInput(uint8_t pin) override {
        pinMode(pin, INPUT);
    }
    
    /**
     * @brief Настройка пина на вход с подтяжкой
     * @param pin Номер пина
     */
    void pinModeInputPullup(uint8_t pin) override {
        pinMode(pin, INPUT_PULLUP);
    }
    
    /**
     * @brief Запись цифрового значения на пин
     * @param pin Номер пина
     * @param value Значение (true/HIGH или false/LOW)
     */
    void digitalWrite(uint8_t pin, bool value) override {
        ::digitalWrite(pin, value ? HIGH : LOW);
    }
    
    /**
     * @brief Чтение цифрового значения с пина
     * @param pin Номер пина
     * @return true если HIGH, false если LOW
     */
    bool digitalRead(uint8_t pin) override {
        return ::digitalRead(pin) == HIGH;
    }
    
    // ========================================
    // Реализация методов задержек
    // ========================================
    
    /**
     * @brief Задержка в микросекундах
     * @param us Количество микросекунд
     */
    void delayMicroseconds(uint32_t us) override {
        ::delayMicroseconds(us);
    }
    
    /**
     * @brief Задержка в миллисекундах
     * @param ms Количество миллисекунд
     */
    void delayMilliseconds(uint32_t ms) override {
        ::delay(ms);
    }
    
    // ========================================
    // Опциональные методы для I2C
    // ========================================
    
    /**
     * @brief Инициализация I2C
     * @param address Адрес устройства на шине I2C
     */
    void i2cInit(uint8_t address) override {
        #if defined(WIRE_H)
        Wire.begin();
        _i2cAddress = address;
        #else
        (void)address;
        #endif
    }
    
    /**
     * @brief Запись байта по I2C
     * @param data Данные для отправки
     */
    void i2cWrite(uint8_t data) override {
        #if defined(WIRE_H)
        Wire.beginTransmission(_i2cAddress);
        Wire.write(data);
        Wire.endTransmission();
        #else
        (void)data;
        #endif
    }
    
    /**
     * @brief Чтение байта по I2C
     * @return Прочитанные данные
     */
    uint8_t i2cRead() override {
        #if defined(WIRE_H)
        Wire.requestFrom(_i2cAddress, (uint8_t)1);
        if (Wire.available()) {
            return Wire.read();
        }
        #endif
        return 0;
    }
    
    /**
     * @brief Запись нескольких байт по I2C
     * @param data Указатель на массив данных
     * @param length Количество байт
     */
    void i2cWriteBytes(const uint8_t* data, uint8_t length) override {
        #if defined(WIRE_H)
        Wire.beginTransmission(_i2cAddress);
        for (uint8_t i = 0; i < length; i++) {
            Wire.write(data[i]);
        }
        Wire.endTransmission();
        #else
        (void)data;
        (void)length;
        #endif
    }
    
    // ========================================
    // Опциональные методы для SPI
    // ========================================
    
    /**
     * @brief Инициализация SPI
     */
    void spiInit() override {
        #if defined(SPI_H)
        SPI.begin();
        #endif
    }
    
    /**
     * @brief Передача байта по SPI
     * @param data Данные для отправки
     * @return Полученные данные
     */
    uint8_t spiTransfer(uint8_t data) override {
        #if defined(SPI_H)
        return SPI.transfer(data);
        #else
        (void)data;
        return 0;
        #endif
    }
    
    // ========================================
    // Утилиты
    // ========================================
    
    /**
     * @brief Получить название платформы
     * @return Строка с названием платформы
     */
    const char* getPlatformName() override {
        return "Arduino";
    }
    
    /**
     * @brief Инициализация HAL
     */
    void init() override {
        // Для Arduino дополнительная инициализация не требуется
        // Arduino framework инициализируется автоматически
    }

private:
    uint8_t _i2cAddress = 0;  ///< Адрес I2C устройства
};

#endif // defined(ARDUINO)

#endif // LCD_HAL_ARDUINO_H