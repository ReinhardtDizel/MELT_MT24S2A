/**
 * @file MELT_MT24S2A.h
 * @brief Универсальная библиотека для LCD дисплея MT24S2A
 * @author Reinhardt Michael
 * @version 2.0.2
 * @date 2024
 * 
 * Полностью автономная версия: не требует внешних HAL-файлов.
 */

#ifndef MELT_MT24S2A_H
#define MELT_MT24S2A_H

#include <stdint.h>
#include <stdbool.h>

// ---- Встроенный HAL для Arduino (без дополнительных файлов) ----
#if defined(ARDUINO)
    #include <Arduino.h>
    class LCD_HAL_Internal {
    public:
        void pinModeOutput(uint8_t pin) { pinMode(pin, OUTPUT); }
        void digitalWrite(uint8_t pin, bool value) { ::digitalWrite(pin, value ? HIGH : LOW); }
        void delayMicroseconds(uint32_t us) { ::delayMicroseconds(us); }
        void delayMilliseconds(uint32_t ms) { ::delay(ms); }
        unsigned long millis() { return ::millis(); }
        void i2cInit(uint8_t) {}   // заглушка
        void i2cWrite(uint8_t) {}  // заглушка
        const char* getPlatformName() { return "Arduino"; }
    };
#else
    #error "Только Arduino поддерживается в этой версии"
#endif

// ---- Команды LCD ----
#define LCD_CLEAR_DISPLAY       0x01
#define LCD_RETURN_HOME         0x02
#define LCD_ENTRY_MODE_SET      0x04
#define LCD_DISPLAY_CONTROL     0x08
#define LCD_CURSOR_SHIFT        0x10
#define LCD_FUNCTION_SET        0x20
#define LCD_SET_CGRAM_ADDR      0x40
#define LCD_SET_DDRAM_ADDR      0x80

#define LCD_ENTRY_RIGHT         0x00
#define LCD_ENTRY_LEFT          0x02
#define LCD_ENTRY_SHIFT_INC     0x01
#define LCD_ENTRY_SHIFT_DEC     0x00

#define LCD_DISPLAY_ON          0x04
#define LCD_DISPLAY_OFF         0x00
#define LCD_CURSOR_ON           0x02
#define LCD_CURSOR_OFF          0x00
#define LCD_BLINK_ON            0x01
#define LCD_BLINK_OFF           0x00

#define LCD_DISPLAY_MOVE        0x08
#define LCD_CURSOR_MOVE         0x00
#define LCD_MOVE_RIGHT          0x04
#define LCD_MOVE_LEFT           0x00

#define LCD_8BIT_MODE           0x10
#define LCD_4BIT_MODE           0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10_DOTS           0x04
#define LCD_5x8_DOTS            0x00

class MELT_MT24S2A {
public:
    // Конструкторы (все содержат rw)
    MELT_MT24S2A(LCD_HAL_Internal* hal, uint8_t rs, uint8_t rw, uint8_t enable,
                 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    MELT_MT24S2A(LCD_HAL_Internal* hal, uint8_t rs, uint8_t rw, uint8_t enable,
                 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    MELT_MT24S2A(LCD_HAL_Internal* hal, uint8_t i2c_address);

    // Простой конструктор (без HAL) – автоматически создаёт внутренний HAL
    MELT_MT24S2A(uint8_t rs, uint8_t rw, uint8_t enable,
                 uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);
    MELT_MT24S2A(uint8_t rs, uint8_t rw, uint8_t enable,
                 uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7);

    ~MELT_MT24S2A();

    // Базовые методы
    void begin(uint8_t cols, uint8_t rows);
    void clear();
    void home();
    void setCursor(uint8_t col, uint8_t row);
    void write(uint8_t c);
    void print(const char* text);
    void print(int number);

    // Управление дисплеем
    void noDisplay();
    void display();
    void noCursor();
    void cursor();
    void noBlink();
    void blink();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void leftToRight();
    void rightToLeft();
    void autoscroll();
    void noAutoscroll();
    void createChar(uint8_t location, uint8_t charmap[8]);

    // Высокоуровневые методы
    void printRus(const char* text);
    void printInt(int32_t num);
    void printIntBlink(int32_t num, int cursorPos, bool show);
    void printVdBlink(int32_t vd_milli, int cursorPos, bool show);
    void clearField(int width);

    unsigned long getMillis();

    const char* getPlatformName();

private:
    LCD_HAL_Internal* _hal;
    uint8_t _rs_pin, _rw_pin, _enable_pin;
    uint8_t _data_pins[8];
    uint8_t _cols, _rows;
    uint8_t _displayfunction, _displaycontrol, _displaymode;
    bool _8bit_mode, _i2c_mode;
    uint8_t _i2c_address;

    void send(uint8_t value, bool isData);
    void write4bits(uint8_t value);
    void pulseEnable();
    void command(uint8_t cmd);
    uint8_t utf8_to_hd44780(const char*& text);
};

#endif
