#include "MELT_MT24S2A.h"
#include <string.h>
#include <stdio.h>

// ---- Конструкторы с внешним HAL ----
MELT_MT24S2A::MELT_MT24S2A(LCD_HAL* hal, uint8_t rs, uint8_t rw, uint8_t enable,
                           uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3,
                           uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
    : _hal(hal), _rs_pin(rs), _rw_pin(rw), _enable_pin(enable),
      _8bit_mode(true), _i2c_mode(false), _i2c_address(0)
{
    _data_pins[0] = d0; _data_pins[1] = d1;
    _data_pins[2] = d2; _data_pins[3] = d3;
    _data_pins[4] = d4; _data_pins[5] = d5;
    _data_pins[6] = d6; _data_pins[7] = d7;
}

MELT_MT24S2A::MELT_MT24S2A(LCD_HAL* hal, uint8_t rs, uint8_t rw, uint8_t enable,
                           uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
    : _hal(hal), _rs_pin(rs), _rw_pin(rw), _enable_pin(enable),
      _8bit_mode(false), _i2c_mode(false), _i2c_address(0)
{
    _data_pins[4] = d4; _data_pins[5] = d5;
    _data_pins[6] = d6; _data_pins[7] = d7;
}

MELT_MT24S2A::MELT_MT24S2A(LCD_HAL* hal, uint8_t i2c_address)
    : _hal(hal), _rs_pin(0), _rw_pin(0), _enable_pin(0),
      _8bit_mode(false), _i2c_mode(true), _i2c_address(i2c_address)
{}

// Деструктор
MELT_MT24S2A::~MELT_MT24S2A() {}

// ---- Инициализация ----
void MELT_MT24S2A::begin(uint8_t cols, uint8_t rows) {
    _cols = cols; _rows = rows;

    if (_i2c_mode) {
        _hal->i2cInit(_i2c_address);
        _displayfunction = LCD_4BIT_MODE | LCD_2LINE | LCD_5x8_DOTS;
    } else {
        _hal->pinModeOutput(_rs_pin);
        _hal->pinModeOutput(_rw_pin);
        _hal->pinModeOutput(_enable_pin);
        if (_8bit_mode) {
            for (int i = 0; i < 8; i++) _hal->pinModeOutput(_data_pins[i]);
            _displayfunction = LCD_8BIT_MODE | LCD_2LINE | LCD_5x8_DOTS;
        } else {
            for (int i = 4; i < 8; i++) _hal->pinModeOutput(_data_pins[i]);
            _displayfunction = LCD_4BIT_MODE | LCD_2LINE | LCD_5x8_DOTS;
        }
    }

    _hal->delayMilliseconds(50);
    if (!_i2c_mode) {
        _hal->digitalWrite(_rs_pin, false);
        _hal->digitalWrite(_rw_pin, false);
        _hal->digitalWrite(_enable_pin, false);
    }

    // Инициализация согласно даташиту
    if (_8bit_mode) {
        command(LCD_FUNCTION_SET | _displayfunction);
        _hal->delayMicroseconds(4500);
        command(LCD_FUNCTION_SET | _displayfunction);
        _hal->delayMicroseconds(150);
        command(LCD_FUNCTION_SET | _displayfunction);
    } else {
        write4bits(0x03);
        _hal->delayMicroseconds(4500);
        write4bits(0x03);
        _hal->delayMicroseconds(4500);
        write4bits(0x03);
        _hal->delayMicroseconds(150);
        write4bits(0x02);
    }

    command(LCD_FUNCTION_SET | _displayfunction);
    _displaycontrol = LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINK_OFF;
    display();
    clear();
    _displaymode = LCD_ENTRY_LEFT | LCD_ENTRY_SHIFT_DEC;
    command(LCD_ENTRY_MODE_SET | _displaymode);
}

// ---- Базовые методы ----
void MELT_MT24S2A::clear() { command(LCD_CLEAR_DISPLAY); _hal->delayMilliseconds(2); }
void MELT_MT24S2A::home()  { command(LCD_RETURN_HOME); _hal->delayMilliseconds(2); }

void MELT_MT24S2A::setCursor(uint8_t col, uint8_t row) {
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    if (row >= _rows) row = _rows - 1;
    command(LCD_SET_DDRAM_ADDR | (col + row_offsets[row]));
}

void MELT_MT24S2A::write(uint8_t c) { send(c, true); }
void MELT_MT24S2A::print(const char* text) { while (*text) write(*text++); }
void MELT_MT24S2A::print(int number) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%d", number);
    print(buf);
}

// ---- Управление дисплеем ----
void MELT_MT24S2A::noDisplay()    { _displaycontrol &= ~LCD_DISPLAY_ON; command(LCD_DISPLAY_CONTROL | _displaycontrol); }
void MELT_MT24S2A::display()      { _displaycontrol |= LCD_DISPLAY_ON;  command(LCD_DISPLAY_CONTROL | _displaycontrol); }
void MELT_MT24S2A::noCursor()     { _displaycontrol &= ~LCD_CURSOR_ON; command(LCD_DISPLAY_CONTROL | _displaycontrol); }
void MELT_MT24S2A::cursor()       { _displaycontrol |= LCD_CURSOR_ON;  command(LCD_DISPLAY_CONTROL | _displaycontrol); }
void MELT_MT24S2A::noBlink()      { _displaycontrol &= ~LCD_BLINK_ON;  command(LCD_DISPLAY_CONTROL | _displaycontrol); }
void MELT_MT24S2A::blink()        { _displaycontrol |= LCD_BLINK_ON;   command(LCD_DISPLAY_CONTROL | _displaycontrol); }
void MELT_MT24S2A::scrollDisplayLeft()  { command(LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_LEFT); }
void MELT_MT24S2A::scrollDisplayRight() { command(LCD_CURSOR_SHIFT | LCD_DISPLAY_MOVE | LCD_MOVE_RIGHT); }
void MELT_MT24S2A::leftToRight()  { _displaymode |= LCD_ENTRY_LEFT;  command(LCD_ENTRY_MODE_SET | _displaymode); }
void MELT_MT24S2A::rightToLeft()  { _displaymode &= ~LCD_ENTRY_LEFT; command(LCD_ENTRY_MODE_SET | _displaymode); }
void MELT_MT24S2A::autoscroll()   { _displaymode |= LCD_ENTRY_SHIFT_INC;  command(LCD_ENTRY_MODE_SET | _displaymode); }
void MELT_MT24S2A::noAutoscroll() { _displaymode &= ~LCD_ENTRY_SHIFT_INC; command(LCD_ENTRY_MODE_SET | _displaymode); }

void MELT_MT24S2A::createChar(uint8_t location, uint8_t charmap[8]) {
    location &= 0x7;
    command(LCD_SET_CGRAM_ADDR | (location << 3));
    for (int i = 0; i < 8; i++) write(charmap[i]);
}

// ---- Внутренние методы ----
void MELT_MT24S2A::command(uint8_t cmd) { send(cmd, false); }

void MELT_MT24S2A::send(uint8_t value, bool isData) {
    if (_i2c_mode) {
        uint8_t data = value;
        if (isData) data |= 0x01;
        _hal->i2cWrite(data);
        return;
    }

    _hal->digitalWrite(_rs_pin, isData);
    _hal->digitalWrite(_rw_pin, false);          // всегда запись
    if (_8bit_mode) {
        for (int i = 0; i < 8; i++) {
            _hal->digitalWrite(_data_pins[i], (value >> i) & 0x01);
        }
        pulseEnable();
    } else {
        write4bits(value >> 4);
        write4bits(value & 0x0F);
    }
}

void MELT_MT24S2A::write4bits(uint8_t value) {
    for (int i = 0; i < 4; i++) {
        _hal->digitalWrite(_data_pins[4 + i], (value >> i) & 0x01);
    }
    pulseEnable();
}

void MELT_MT24S2A::pulseEnable() {
    _hal->digitalWrite(_enable_pin, false);
    _hal->delayMicroseconds(1);
    _hal->digitalWrite(_enable_pin, true);
    _hal->delayMicroseconds(1);
    _hal->digitalWrite(_enable_pin, false);
    _hal->delayMicroseconds(100);
}

// ---- Высокоуровневые методы ----
uint8_t MELT_MT24S2A::utf8_to_hd44780(const char*& text) {
    // Полная реализация, скопированная из вашей старой версии (без Arduino)
    uint8_t c = (uint8_t)*text;
    if (c < 0x80) { text++; return c; }
    if (c == 0xD0 || c == 0xD1) {
        uint8_t c2 = (uint8_t)*(text + 1);
        uint16_t ucode = (c == 0xD0) ? 0x0410 + (c2 - 0x90) : 0x0420 + (c2 - 0x80);
        text += 2;
        switch (ucode) {
            case 0x0410: return 0x41; case 0x0430: return 0x61;
            case 0x0411: return 0xA0; case 0x0431: return 0xB2;
            case 0x0412: return 0x42; case 0x0432: return 0xB3;
            case 0x0413: return 0xA1; case 0x0433: return 0xB4;
            case 0x0414: return 0xE0; case 0x0434: return 0xE3;
            case 0x0415: return 0x45; case 0x0435: return 0x65;
            case 0x0401: return 0xA2; case 0x0451: return 0xB5;
            case 0x0416: return 0xA3; case 0x0436: return 0xB6;
            case 0x0417: return 0xA4; case 0x0437: return 0xB7;
            case 0x0418: return 0xA5; case 0x0438: return 0xB8;
            case 0x0419: return 0xA6; case 0x0439: return 0xB9;
            case 0x041A: return 0x4B; case 0x043A: return 0xBA;
            case 0x041B: return 0xA7; case 0x043B: return 0xBB;
            case 0x041C: return 0x4D; case 0x043C: return 0xBC;
            case 0x041D: return 0x48; case 0x043D: return 0xBD;
            case 0x041E: return 0x4F; case 0x043E: return 0x6F;
            case 0x041F: return 0xA8; case 0x043F: return 0xBE;
            case 0x0420: return 0x50; case 0x0440: return 0x70;
            case 0x0421: return 0x43; case 0x0441: return 0x63;
            case 0x0422: return 0x54; case 0x0442: return 0xBF;
            case 0x0423: return 0xA9; case 0x0443: return 0x79;
            case 0x0424: return 0xAA; case 0x0444: return 0xE4;
            case 0x0425: return 0x58; case 0x0445: return 0x78;
            case 0x0426: return 0xE1; case 0x0446: return 0xE5;
            case 0x0427: return 0xAB; case 0x0447: return 0xC0;
            case 0x0428: return 0xAC; case 0x0448: return 0xC1;
            case 0x0429: return 0xE2; case 0x0449: return 0xE6;
            case 0x042A: return 0xAD; case 0x044A: return 0xC2;
            case 0x042B: return 0xAE; case 0x044B: return 0xC3;
            case 0x042C: return 0x62; case 0x044C: return 0xC4;
            case 0x042D: return 0xAF; case 0x044D: return 0xC5;
            case 0x042E: return 0xB0; case 0x044E: return 0xC6;
            case 0x042F: return 0xB1; case 0x044F: return 0xC7;
            default: return '?';
        }
    }
    text++;
    return '?';
}

void MELT_MT24S2A::printRus(const char* text) {
    while (*text) {
        uint8_t ch = utf8_to_hd44780(text);
        write(ch);
        _hal->delayMicroseconds(50);
    }
}

void MELT_MT24S2A::printInt(int32_t num) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%ld", num);
    print(buf);
}

void MELT_MT24S2A::printIntBlink(int32_t num, int cursorPos, bool show) {
    char buf[12];
    snprintf(buf, sizeof(buf), "%ld", num);
    int len = strlen(buf);
    if (cursorPos >= len) cursorPos = len - 1;
    for (int i = 0; i < len; i++) {
        write(i == cursorPos && !show ? ' ' : buf[i]);
    }
}

void MELT_MT24S2A::printVdBlink(int32_t vd_milli, int cursorPos, bool show) {
    int whole = vd_milli / 1000;
    int frac  = vd_milli % 1000;
    char buf[8];
    snprintf(buf, sizeof(buf), "%d.%03d", whole, frac);
    int len = strlen(buf);
    if (cursorPos >= len) cursorPos = len - 1;
    for (int i = 0; i < len; i++) {
        write(i == cursorPos && !show ? ' ' : buf[i]);
    }
}

void MELT_MT24S2A::clearField(int width) {
    for (int i = 0; i < width; i++) write(' ');
}

unsigned long MELT_MT24S2A::getMillis() {
    return _hal->millis();
}

const char* MELT_MT24S2A::getPlatformName() {
    return _hal->getPlatformName();
}