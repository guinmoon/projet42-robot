# Рефакторинг обработки датчиков

## Проблема
В исходном коде было слишком много переменных с префиксами `left` и `right` для обработки левого и правого датчиков, а также дублирование логики в методах `LeftSensorsTask()` и `RightSensorsTask()`.

## Решение

### 1. Создан класс `SensorHandler`
**Файлы:**
- `include/sensor_handler.hpp` - заголовок класса
- `src/sensor_handler.cpp` - реализация класса

**Назначение:** Инкапсуляция всех данных и логики обработки одного датчика.

**Инкапсулированные данные:**
- `Adafruit_VL53L0X sensor` - сам датчик
- `VL53L0X_RangingMeasurementData_t distanceMeasure` - данные измерения
- `uint16_t sensorValues[WINDOW_SIZE]` - скользящее окно для усреднения
- `int sensorValueIndex` - индекс в окне
- `bool isHandNear` - флаг близости руки
- `unsigned long ocupStartTime` - время начала поднесения
- `bool longAttnBegin` - флаг начала длинного внимания
- `bool isRightSensor` - идентификатор датчика (левый/правый)
- `Proj42Events* events` - указатель на обработчик событий

**Методы:**
- `SensorHandler(Proj42Events* _events, bool _isRight)` - конструктор
- `bool initSensor()` - инициализация датчика
- `void sensorTask()` - **общий метод** для обработки обоих датчиков (заменяет `LeftSensorsTask()` и `RightSensorsTask()`)
- `bool getLongAttnBegin()` / `void setLongAttnBegin(bool)` - геттер/сеттер для флага

### 2. Обновлен класс `Proj42Events`
**Изменения:**
- Удалены все переменные с префиксами `left` и `right`
- Добавлены указатели на обработчики: `SensorHandler* leftSensorHandler` и `SensorHandler* rightSensorHandler`
- Методы `LeftSensorsTask()` и `RightSensorsTask()` теперь просто делегируют вызов в `sensorTask()` соответствующего обработчика
- Добавлен деструктор для очистки памяти

## Преимущества

1. **Устранение дублирования кода** - один метод `sensorTask()` вместо двух идентичных
2. **Инкапсуляция** - все данные датчика собраны в одном классе
3. **Легкость расширения** - для добавления нового датчика нужно только создать новый `SensorHandler`
4. **Упрощение поддержки** - изменения в логике обработки датчика вносятся в одном месте
5. **Более чистый код** - уменьшено количество переменных в классе `Proj42Events`

## Пример использования

```cpp
// В конструкторе Proj42Events:
leftSensorHandler = new SensorHandler(this, false);   // false = левый датчик
rightSensorHandler = new SensorHandler(this, true);   // true = правый датчик

// В методах задач:
void Proj42Events::LeftSensorsTask()
{
    leftSensorHandler->sensorTask();  // Общий метод для обоих датчиков
}

void Proj42Events::RightSensorsTask()
{
    rightSensorHandler->sensorTask(); // Тот же метод, но для правого датчика
}
```

## Примечания
- `SensorHandler` автоматически вызывает соответствующие методы `leftDistanceShortAttn()`, `leftDistanceLongAttn()`, `rightDistanceShortAttn()`, `rightDistanceLongAttn()` в зависимости от того, левый это датчик или правый
- Все константы порогов и временных интервалов инкапсулированы внутри `SensorHandler`
- Логика усреднения и обработки скользящего окна также инкапсулирована