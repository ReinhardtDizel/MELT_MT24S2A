# MELT_MT24S2A

**Универсальная библиотека для управления LCD дисплеем MT24S2A (HD44780-совместимые).**

## Особенности

- **Полная кроссплатформенность** – ядро не зависит от Arduino/AVR/STM32/ESP.
- **Поддержка 8-битного, 4-битного и I2C-режимов**.
- **Русский язык** – автоматическое преобразование UTF-8 в кодовую страницу HD44780.
- **Удобные методы** `printInt`, `printVdBlink`, `clearField` и др.
- **Аппаратная абстракция (HAL)** – вы легко адаптируете библиотеку под свою платформу.

## Структура

- `src/` – основной код (независим от платформы).
- `examples/Arduino_HAL/` – готовая реализация HAL для Arduino.
- `examples/AVR_HAL/` – готовая реализация HAL для чистого AVR (с собственным таймером).

## Подключение

Вам необходимо реализовать класс, наследующий `LCD_HAL`, или взять один из примеров.

### Пример для Arduino

Скопируйте `examples/Arduino_HAL/LCD_HAL_Arduino.h` в ваш проект, подключите и передайте в конструктор дисплея:

```cpp
#include "LCD_HAL_Arduino.h"
#include "MELT_MT24S2A.h"

LCD_HAL_Arduino hal;
MELT_MT24S2A lcd(&hal, 12, 11, 10, 5, 4, 3, 2); // 4-битный режим

void setup() {
    lcd.begin(16, 2);
    lcd.printRus("Привет!");
}
Пример для AVR

Используйте examples/AVR_HAL/LCD_HAL_Avr.h. Не забудьте вызвать hal.init() перед lcd.begin():

```cpp
#include "LCD_HAL_Avr.h"
#include "MELT_MT24S2A.h"

LCD_HAL_Avr hal;
MELT_MT24S2A lcd(&hal, 12, 11, 10, 5, 4, 3, 2);

int main() {
    hal.init();  // инициализация таймера для millis()
    lcd.begin(16, 2);
    lcd.printRus("AVR работает!");
    while(1);
}
```

Зависимости

· Ядро не имеет внешних зависимостей.
· Для примеров требуется либо Arduino-фреймворк, либо AVR-библиотеки (avr-libc).

Лицензия

MIT License.

Автор

Reinhardt Michael
GitHub: ReinhardtDizel
Email: dizel882@gmail.com
