/**
 * @file LCD_HAL.h
 * @brief Hardware Abstraction Layer для библиотеки MELT_MT24S2A
 * @author Your Name
 * @version 1.0.0
 * @date 2024
 * 
 * Базовый интерфейс для работы с различными платформами
 */

#ifndef LCD_HAL_H
#define LCD_HAL_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @class LCD_HAL
 * @brief Абстрактный базовый класс для аппаратной абстракции
 * 
 * Этот класс определяет интерфейс для работы с GPIO и задержками
 * на различных платформах (Arduino, AVR, STM32, ESP)
 */
class LCD_HAL {
public:
    /**
     * @brief Виртуальный деструктор
     */
    virtual ~LCD_HAL() {}
    
    // ========================================
    // Методы работы с GPIO
    // ========================================
    
    /**
     * @brief Настройка пина на выход
     * @param pin Номер пина
     */
    virtual void pinModeOutput(uint8_t pin) = 0;
    
    /**
     * @brief Настройка пина на вход
     * @param pin Номер пина
     */
    virtual void pinModeInput(uint8_t pin) = 0;
    
    /**
     * @brief Настройка пина на вход с подтяжкой
     * @param pin Номер пина
     */
    virtual void pinModeInputPullup(uint8_t pin) = 0;
    
    /**
     * @brief Запись цифрового значения на пин
     * @param pin Номер пина
     * @param value Значение (true/HIGH или false/LOW)
     */
    virtual void digitalWrite(uint8_t pin, bool value) = 0;
    
    /**
     * @brief Чтение цифрового значения с пина
     * @param pin Номер пина
     * @return true если HIGH, false если LOW
     */
    virtual bool digitalRead(uint8_t pin) = 0;
    
    // ========================================
    // Методы задержек
    // ========================================
    
    /**
     * @brief Задержка в микросекундах
     * @param us Количество микросекунд
     */
    virtual void delayMicroseconds(uint32_t us) = 0;
    
    /**
     * @brief Задержка в миллисекундах
     * @param ms Количество миллисекунд
     */
    virtual void delayMilliseconds(uint32_t ms) = 0;
    
    // ========================================
    // Опциональные методы для I2C
    // (Если ваш дисплей использует I2C)
    // ========================================
    
    /**
     * @brief Инициализация I2C
     * @param address Адрес устройства на шине I2C
     */
    virtual void i2cInit(uint8_t address) { (void)address; }
    
    /**
     * @brief Запись байта по I2C
     * @param data Данные для отправки
     */
    virtual void i2cWrite(uint8_t data) { (void)data; }
    
    /**
     * @brief Чтение байта по I2C
     * @return Прочитанные данные
     */
    virtual uint8_t i2cRead() { return 0; }
    
    /**
     * @brief Запись нескольких байт по I2C
     * @param data Указатель на массив данных
     * @param length Количество байт
     */
    virtual void i2cWriteBytes(const uint8_t* data, uint8_t length) { 
        (void)data; 
        (void)length; 
    }
    
    // ========================================
    // Опциональные методы для SPI
    // (Если ваш дисплей использует SPI)
    // ========================================
    
    /**
     * @brief Инициализация SPI
     */
    virtual void spiInit() {}
    
    /**
     * @brief Передача байта по SPI
     * @param data Данные для отправки
     * @return Полученные данные
     */
    virtual uint8_t spiTransfer(uint8_t data) { 
        (void)data; 
        return 0; 
    }
    
    // ========================================
    // Утилиты
    // ========================================
    
    /**
     * @brief Получить название платформы
     * @return Строка с названием платформы
     */
    virtual const char* getPlatformName() = 0;
    
    /**
     * @brief Инициализация HAL (если требуется)
     */
    virtual void init() {}
};

#endif // LCD_HAL_H