# ESP Dimmer

Многоканальный регулятор яркости на ESP8266/ESP32.

## Цель

- 4 независимых канала (кнопка + PWM на канал);
- 1 master-кнопка (общее вкл/выкл);
- PWM 1 кГц, яркость 10–100%;
- после перезапуска все каналы выключены;
- дальше — ESP32-C3 + оптроны.

## Платформы

| | Статус | Env PlatformIO |
|---|---|---|
| **NodeMCU ESP8266** | Legacy, протестированный стенд | `nodemcuv2` |
| **ESP32-C3 SuperMini** | Target (кандидат распиновки — сверить перед пайкой) | `esp32c3` |

Схемы раздельно: [docs/wiring.md](docs/wiring.md).

Версии platform зафиксированы в `platformio.ini` (`espressif8266 @ 4.2.1`, `espressif32 @ 7.0.1`).

## Управление каналом

| Действие | Результат |
|---|---|
| Короткое нажатие (`< 450 мс`) | Вкл/выкл; яркость в RAM |
| Удержание (канал ON) | Первое после старта — уменьшение; далее чередуется |
| Удержание из OFF | Вкл с 10% и увеличение |

## Master

- хотя бы один канал ON → выключить все;
- все OFF → включить все на сохранённой яркости;
- при toggle во время удержания канала fade останавливается, яркость замораживается; ввод канала игнорируется до отпускания кнопки.

Все кнопки → **GND**, `INPUT_PULLUP`. На каждом PWM: **pull-down 10 кОм к GND** + токоограничение.

## Структура

```text
esp-dimmer/
├── platformio.ini
├── src/
│   ├── main.cpp
│   ├── dimmer_channel.cpp
│   └── pwm_backend.cpp
├── include/
│   ├── config.h
│   ├── dimmer_channel.h
│   └── pwm_backend.h
├── docs/
│   └── wiring.md
└── README.md
```

---

## Быстрый старт с нуля

### 1. Что нужно

- **Git**
- **Python 3.10+** (для PlatformIO CLI)
- Плата: NodeMCU **или** ESP32-C3 SuperMini
- **USB-кабель с data** (не только зарядка)
- Редактор по желанию: [Cursor](https://cursor.com/) / VS Code + расширение PlatformIO, либо только CLI

### 2. Клонирование

```bash
git clone git@github.com:Web-Usov/esp-dimmer.git
cd esp-dimmer
```

HTTPS:

```bash
git clone https://github.com/Web-Usov/esp-dimmer.git
cd esp-dimmer
```

### 3. Установка PlatformIO

**Вариант A — CLI (рекомендуется для скриптов и CI-подобных шагов):**

```bash
python -m pip install -U pip
python -m pip install "platformio==6.1.19"
```

Проверка:

```bash
pio --version
```

На Windows иногда удобнее вызывать полный путь:

```text
%USERPROFILE%\.platformio\penv\Scripts\pio.exe
```

**Вариант B — IDE:** установить расширение `platformio.platformio-ide` в Cursor/VS Code. Оно подтянет PlatformIO Core само; команды ниже те же в терминале IDE.

При первой сборке PlatformIO скачает toolchain и framework (нужен интернет, один раз может занять несколько минут).

### 4. Сборка (без платы)

Из корня репозитория:

```bash
# Legacy NodeMCU
pio run -e nodemcuv2

# Target ESP32-C3
pio run -e esp32c3
```

Успех: `SUCCESS` и бинарники в `.pio/build/<env>/firmware.bin`.

Default env в `platformio.ini` — `nodemcuv2` (`pio run` без `-e` соберёт его).

### 5. Поиск порта платы

Подключите плату по USB и выполните:

```bash
pio device list
```

Запомните COM-порт (Windows: `COMx`) или `/dev/ttyUSB*` / `/dev/ttyACM*` (Linux/macOS).

**ESP32-C3 SuperMini (native USB):** в системе обычно появляется устройство вида *USB Serial Device (COMx)* / *USB JTAG/serial* (`VID_303A`). Если порта нет:

1. Только USB, **без** внешнего 5V одновременно (по описанию многих SuperMini — USB или внешнее питание).
2. Другой data-кабель, порт напрямую в ПК.
3. Режим прошивки: зажать **BOOT** → нажать/отпустить **RESET** → отпустить **BOOT**. Должен появиться звук подключения устройства и COM-порт.

### 6. Прошивка

Подставьте свой порт.

**NodeMCU:**

```bash
pio run -e nodemcuv2 -t upload --upload-port COMx
```

**ESP32-C3:**

```bash
pio run -e esp32c3 -t upload --upload-port COMx
```

Без `--upload-port` PlatformIO попробует угадать порт сам.

Если upload на C3 обрывается — снова войдите в download mode (BOOT+RESET) и повторите команду, пока порт стабилен.

Успех: `Chip is ESP32-C3` / `Hash of data verified` / `SUCCESS`.

### 7. Serial-монитор

```bash
pio device monitor -b 115200 --port COMx
```

Или:

```bash
pio run -e esp32c3 -t monitor --upload-port COMx
```

После boot ожидаются строки вроде `esp-dimmer booted`, `channels: 4 + master`. Выход из монитора: `Ctrl+C`.

На NodeMCU master сидит на `D9`/`RX` — **ввод** в Serial с ПК недоступен; лог с платы через TX работает.

### 8. Типовой цикл разработки

```bash
pio run -e esp32c3 -t upload --upload-port COMx
pio device monitor -b 115200 --port COMx
```

Пины и поведение кнопок — в `include/config.h` и [docs/wiring.md](docs/wiring.md).

---

## IDE: clangd / compile_commands

Рекомендуется расширение `llvm-vs-code-extensions.vscode-clangd` (см. `.vscode/extensions.json`). Пакет C/C++ от Microsoft лучше не ставить рядом (`unwantedRecommendations`).

После установки PlatformIO сгенерируйте compilation database:

```bash
pio run -e esp32c3 -t compiledb
# или
pio run -e nodemcuv2 -t compiledb
```

В Cursor/VS Code: Tasks → `compiledb: esp32c3` / `compiledb: nodemcuv2`.

`compile_commands.json` в `.gitignore` — локальный артефакт.

---

## CI

На `push` / `pull_request` GitHub Actions собирает оба env (`.github/workflows/build.yml`, PlatformIO `6.1.19`) — только compile, без прошивки.
