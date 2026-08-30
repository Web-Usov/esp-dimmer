# ESP Dimmer

Многоканальный регулятор яркости на ESP8266/ESP32.

## Цель

- 4 независимых канала (кнопка + PWM на канал);
- 1 master-кнопка (общее вкл/выкл);
- PWM 1 кГц, яркость 10–100%;
- после перезапуска все каналы выключены;
- дальше — ESP32-C3 + оптроны.

## Платформы

| | Статус |
|---|---|
| **NodeMCU ESP8266** | Legacy, протестированный стенд |
| **ESP32-C3 SuperMini** | Target (кандидат распиновки — сверить перед пайкой) |

Схемы раздельно: [docs/wiring.md](docs/wiring.md).

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

## Сборка

Версии platform зафиксированы в `platformio.ini`.

```bash
pio run -e nodemcuv2      # legacy NodeMCU
pio run -e esp32c3        # target ESP32-C3
pio run -e nodemcuv2 -t upload
pio device monitor        # 115200
```

### clangd / compile_commands

Рекомендуется расширение `llvm-vs-code-extensions.vscode-clangd`.

После установки PlatformIO сгенерировать compilation database (Tasks: `compiledb: nodemcuv2` или `compiledb: esp32c3`):

```bash
pio run -e esp32c3 -t compiledb
# или
pio run -e nodemcuv2 -t compiledb
```

`compile_commands.json` в `.gitignore` — локальный артефакт.
