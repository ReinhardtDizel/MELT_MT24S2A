/**
 * @brief Пример использования библиотеки MELT_MT24S2A
 * 
 * Демонстрация работы дисплея MT24S2A в связке с ШИМ-выходом 0-10 В.
 * Кнопки: A4 → сброс частоты в 0, A5 → уменьшить уставку на 0.1 Гц, A3 → увеличить на 0.1 Гц.
 * Плавное изменение частоты с шагом 10 Гц/с.
 */

#include <Arduino.h>
#include <MELT_MT24S2A.h>                // Основной заголовок библиотеки

// ---------- Пины (как в вашем рабочем проекте) ----------
const uint8_t RS = 6, EN = 8;
const uint8_t D0 = 9, D1 = 10, D2 = 11, D3 = 12;
const uint8_t D4 = 13, D5 = 14, D6 = 15, D7 = 16;
const uint8_t BTN1 = A4, BTN2 = A5, BTN3 = A3;

// ---------- Глобальный объект дисплея ----------
// Используется автоматический HAL для Arduino
LCD_HAL_Platform hal;                     // LCD_HAL_Arduino (определён в MELT_MT24S2A.h)
MELT_MT24S2A lcd(&hal, RS, EN, D0, D1, D2, D3, D4, D5, D6, D7);

// ---------- Класс PWMOutput (точная копия из вашего проекта) ----------
class PWMOutput {
  uint8_t _pin;
  float _minFreq, _maxFreq, _currentFreq, _targetFreq, _maxStepPerSec;
  unsigned long _lastUpdate;
public:
  PWMOutput(uint8_t pin, float minFreq, float maxFreq, float maxStepPerSec)
    : _pin(pin), _minFreq(minFreq), _maxFreq(maxFreq), _maxStepPerSec(maxStepPerSec),
      _currentFreq(minFreq), _targetFreq(minFreq), _lastUpdate(0) {}
  void begin() {
    pinMode(_pin, OUTPUT);
    analogWrite(_pin, 0);
    _lastUpdate = millis();
  }
  void setFrequency(float freq) {
    if (freq < _minFreq) freq = _minFreq;
    if (freq > _maxFreq) freq = _maxFreq;
    _targetFreq = freq;
  }
  void update() {
    unsigned long now = millis();
    unsigned long dt = now - _lastUpdate;
    if (dt < 10) return;
    _lastUpdate = now;
    float maxDelta = _maxStepPerSec * (dt / 1000.0);
    if (maxDelta < 0.001) maxDelta = 0.001;
    float delta = _targetFreq - _currentFreq;
    if (delta > maxDelta) delta = maxDelta;
    if (delta < -maxDelta) delta = -maxDelta;
    if (abs(delta) > 0.0005) {
      _currentFreq += delta;
      if (_currentFreq < _minFreq) _currentFreq = _minFreq;
      if (_currentFreq > _maxFreq) _currentFreq = _maxFreq;
      float voltage = (_currentFreq - _minFreq) / (_maxFreq - _minFreq) * 10.0;
      int32_t pwmValue = (int32_t)(voltage / 10.0 * 255.0);
      analogWrite(_pin, pwmValue);
    }
  }
  float getCurrentFrequency() const { return _currentFreq; }
};

PWMOutput driveOutput(5, 0.0, 50.0, 10.0);   // пин 5, 0-50 Гц, 10 Гц/с

// ---------- Глобальные переменные для отображения ----------
float currentFreq = 0.0;
float targetFreq = 0.0;
float voltage = 0.0;

// ---------- Кнопки с защитой от дребезга (как в проекте, но проще) ----------
class DebouncedButton {
  uint8_t pin;
  bool lastState;
  unsigned long lastTime;
public:
  DebouncedButton(uint8_t p) : pin(p), lastState(HIGH), lastTime(0) {
    pinMode(pin, INPUT_PULLUP);
  }
  bool pressed() {
    bool state = digitalRead(pin);
    if (state == LOW && lastState == HIGH && millis() - lastTime > 50) {
      lastState = LOW;
      lastTime = millis();
      return true;
    }
    lastState = state;
    return false;
  }
};

DebouncedButton btn1(BTN1), btn2(BTN2), btn3(BTN3);

// ---------- Вспомогательная функция для вывода float с двумя знаками после запятой ----------
void printFloat2(float value) {
  int whole = (int)value;
  int frac = (int)((value - whole) * 100 + 0.5);
  if (frac < 0) frac = -frac;  // на всякий случай
  lcd.printInt(whole);
  lcd.write('.');
  if (frac < 10) lcd.write('0');
  lcd.printInt(frac);
}

// ---------- SETUP ----------
void setup() {
  lcd.begin(24, 2);               // Инициализация: 24 символа, 2 строки
  lcd.clear();

  // Приветствие с русским текстом (через UTF-8, библиотека сама перекодирует)
  lcd.setCursor(0, 0);
  lcd.printRus("Библиотека MELT");
  lcd.setCursor(0, 1);
  lcd.printRus("MT24S2A тест");
  delay(1500);

  lcd.clear();
  driveOutput.begin();
  targetFreq = 0.0;
  driveOutput.setFrequency(targetFreq);
}

// ---------- LOOP ----------
void loop() {
  // Обработка кнопок (как в вашем тесте)
  if (btn1.pressed()) {             // Кнопка 1 – сброс в 0 Гц
    targetFreq = 0.0;
    driveOutput.setFrequency(targetFreq);
  }
  if (btn2.pressed()) {             // Кнопка 2 – уменьшить на 0.1 Гц
    targetFreq -= 0.1;
    if (targetFreq < 0.0) targetFreq = 0.0;
    driveOutput.setFrequency(targetFreq);
  }
  if (btn3.pressed()) {             // Кнопка 3 – увеличить на 0.1 Гц
    targetFreq += 0.1;
    if (targetFreq > 50.0) targetFreq = 50.0;
    driveOutput.setFrequency(targetFreq);
  }

  // Плавное обновление ШИМ
  driveOutput.update();
  currentFreq = driveOutput.getCurrentFrequency();

  // Вывод информации на дисплей (аналог вашего старого кода)
  lcd.setCursor(0, 0);
  lcd.print("F=");
  printFloat2(currentFreq);
  lcd.print("  ");

  lcd.setCursor(11, 0);
  lcd.print("Set=");
  printFloat2(targetFreq);
  lcd.print("  ");

  // Напряжение вычисляем так же, как в классе PWMOutput
  voltage = (currentFreq - 0.0) / 50.0 * 10.0;
  lcd.setCursor(0, 1);
  lcd.print("U=");
  printFloat2(voltage);
  lcd.print("V  ");

  lcd.setCursor(11, 1);
  lcd.print("PWM=");
  int32_t pwmVal = (int32_t)(voltage / 10.0 * 255.0);
  lcd.printInt(pwmVal);
  lcd.print("   ");

  delay(10);
}