# ESP Dimmer

Многоканальный регулятор яркости на ESP32-C3 (primary) с совместимостью ESP8266 (legacy).

## Цель

- 4 независимых канала (кнопка + PWM на канал);
- 1 master-кнопка (общее вкл/выкл);
- PWM 1 кГц, яркость 10–100%;
- после перезапуска все каналы выключены;
- текущий этап — переход от тестовых LED на GPIO к управлению реальной нагрузкой через `PC817 -> BC548 -> DIM` драйвера LDH-25-350W.

**План текущего hardware-этапа:** [docs/load-control-plan.md](docs/load-control-plan.md).

## Платформы

| | Статус | Env PlatformIO |
|---|---|---|
| **ESP32-C3 SuperMini** | **Primary / hardware validated** | `esp32c3` (default) |
| **NodeMCU ESP8266** | Legacy / compatibility | `nodemcuv2` |

Схемы контроллерного стенда: [docs/wiring.md](docs/wiring.md) — сначала C3, затем legacy NodeMCU.

Версии platform зафиксированы в `platformio.ini` (`espressif32 @ 7.0.1`, `espressif8266 @ 4.2.1`).

### Support policy

- Новые возможности реализуются **в первую очередь для ESP32-C3**.
- Для ESP8266 гарантируем **сборку в CI** и исправление **критических регрессий**; полный feature parity не обязателен.
- **GitHub Releases** публикуют артефакты только для ESP32-C3 (тег `v*`, workflow `release.yml`).
- Обычный CI (`build.yml`) собирает оба env: `esp32c3` + `nodemcuv2`.

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
│   ├── AGENTS.md
│   ├── wiring.md
│   ├── load-control-plan.md
│   ├── requirements.txt
│   ├── schematics/
│   │   ├── render_all.py
│   │   ├── controller_stand.py
│   │   ├── system_overview.py
│   │   ├── load_control_channel.py
│   │   └── four_channel_overview.py
│   └── assets/schematics/
│       ├── system-overview.svg
│       ├── controller-stand.svg
│       ├── load-control-channel.svg
│       └── four-channel-overview.svg
└── README.md
```

Схемы документации: `python docs/schematics/render_all.py` (Schemdraw → SVG).

---

## Быстрый старт с нуля

### 1. Что нужно

- **Git**
- **Python 3.10+** (для PlatformIO CLI)
- Плата: **ESP32-C3 SuperMini** (primary) или NodeMCU ESP8266 (legacy)
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

Default env — `esp32c3` (`pio run` без `-e` собирает primary):

```bash
pio run                 # ESP32-C3 (default)
pio run -e esp32c3      # то же явно
pio run -e nodemcuv2    # legacy NodeMCU
```

Успех: `SUCCESS` и бинарники в `.pio/build/<env>/firmware.bin`.

### 5. Поиск порта платы

Подключите плату по USB и выполните:

```bash
pio device list
```

Запомните COM-порт (Windows: `COMx`) или `/dev/ttyUSB*` / `/dev/ttyACM*` (Linux/macOS).

**ESP32-C3 SuperMini (native USB):** в системе обычно появляется устройство вида *USB Serial Device (COMx)* / *USB JTAG/serial* (`VID_303A`). В `platformio.ini` для `esp32c3` включены `ARDUINO_USB_MODE` и `ARDUINO_USB_CDC_ON_BOOT`, чтобы `Serial` шёл в этот USB CDC (а не в UART0 на GPIO20/21 — master на GPIO20). Если порта нет:

1. Не подключать одновременно USB и внешний источник питания, пока power-path конкретной SuperMini не проверен.
2. Другой data-кабель, порт напрямую в ПК.
3. Режим прошивки: зажать **BOOT** → нажать/отпустить **RESET** → отпустить **BOOT**. Должен появиться звук подключения устройства и COM-порт.

### 6. Прошивка

Подставьте свой порт.

**ESP32-C3 (primary):**

```bash
pio run -t upload --upload-port COMx
# или явно:
pio run -e esp32c3 -t upload --upload-port COMx
```

**NodeMCU (legacy):**

```bash
pio run -e nodemcuv2 -t upload --upload-port COMx
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
pio run -t monitor --monitor-port COMx
```

После boot ожидаются строки вроде `esp-dimmer booted`, `channels: 4 + master`. Выход из монитора: `Ctrl+C`.

На NodeMCU master сидит на `D9`/`RX` — **ввод** в Serial с ПК недоступен; лог с платы через TX работает.

### 8. Типовой цикл разработки (C3)

```bash
pio run -t upload --upload-port COMx
pio device monitor -b 115200 --port COMx
```

Пины и поведение кнопок — в `include/config.h` и [docs/wiring.md](docs/wiring.md).

---

## IDE: clangd / compile_commands

Рекомендуется расширение `llvm-vs-code-extensions.vscode-clangd` (см. `.vscode/extensions.json`). Пакет C/C++ от Microsoft лучше не ставить рядом (`unwantedRecommendations`).

После установки PlatformIO сгенерируйте compilation database:

```bash
pio run -t compiledb
# или явно:
pio run -e esp32c3 -t compiledb
pio run -e nodemcuv2 -t compiledb
```

В Cursor/VS Code: Tasks → `compiledb: esp32c3` / `compiledb: nodemcuv2`.

`compile_commands.json` в `.gitignore` — локальный артефакт.

---

## CI и Releases

- **CI** (`push` / `pull_request`): оба env — `.github/workflows/build.yml`, PlatformIO `6.1.19` (compile-only).
- **Releases** (тег `v*` или `workflow_dispatch`): только ESP32-C3 — `.github/workflows/release.yml` (`firmware.bin`, merged `full.bin`, `SHA256SUMS.txt`).
