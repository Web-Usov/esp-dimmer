# Схемы подключения

Пока живут **две** реализации: legacy NodeMCU (протестирована) и target ESP32-C3 (кандидат пинов).

- [Legacy: NodeMCU ESP8266](#legacy-nodemcu-esp8266)
- [Target: ESP32-C3 SuperMini](#target-esp32-c3-supermini)
- [Общее: LED / оптрон / master / проверка](#общее)

---

## Legacy: NodeMCU ESP8266

Протестированный стенд. Boot-strap на `D3`/`D4` приемлем: не держать BTN1/BTN2 при питании/Reset.

Все кнопки — **active LOW → GND**, `INPUT_PULLUP`.

### Распиновка

| Назначение | LED / PWM | Кнопка |
|---|---|---|
| CH1 | `D1` | `D3` → GND |
| CH2 | `D2` | `D4` → GND |
| CH3 | `D5` | `D7` → GND |
| CH4 | `D8` | `D6` → GND |
| MASTER | — | `D9` / `RX` → GND |

`D0` свободен. Serial — только вывод через `TX`.

### Схема

```text
                         NodeMCU ESP8266
                      ┌─────────────────────┐
                   D1 ●──┬── 10k ── GND          pull-down
                      │  └── Rled ──►|── GND     LED ch1
                   D2 ●──┬── 10k ── GND
                      │  └── Rled ──►|── GND     LED ch2
                   D5 ●──┬── 10k ── GND
                      │  └── Rled ──►|── GND     LED ch3
                   D8 ●──┬── 10k ── GND
                      │  └── Rled ──►|── GND     LED ch4
                      │
                   D3 ●── [BTN1] ── GND
                   D4 ●── [BTN2] ── GND
                   D7 ●── [BTN3] ── GND
                   D6 ●── [BTN4] ── GND
                   D9 ●── [MASTER] ── GND
                      │
                  GND ●──────────────────── общий GND
                   TX ●  Serial out
                      └─────────────────────┘

Питание: micro-USB
Rled ≈ 330–470 Ω (на тесте допустим 10 кОм — будет тускло)
```

Замечания NodeMCU:

1. `D8` — LED (OFF = LOW), не кнопка: на GPIO15 сильный board pull-down.
2. `D9`/`RX` — master; ввод в Serial с ПК недоступен.
3. `D0` не для кнопок: нет обычного `INPUT_PULLUP`.

---

## Target: ESP32-C3 SuperMini

**Кандидат** распиновки (сверить с конкретной SuperMini перед пайкой).  
Кнопки не на boot-strap и не на native USB D+/D− (GPIO18/19).

Все кнопки — **active LOW → GND**, `INPUT_PULLUP`.  
PWM — **active HIGH**, 1 кГц, яркость 10–100%, boot = все OFF.

### Распиновка (кандидат)

| Назначение | LED / PWM | Кнопка |
|---|---|---|
| CH1 | GPIO0 | GPIO5 → GND |
| CH2 | GPIO1 | GPIO6 → GND |
| CH3 | GPIO3 | GPIO7 → GND |
| CH4 | GPIO4 | GPIO10 → GND |
| MASTER | — | GPIO20 → GND |

### Схема

```text
                      ESP32-C3 SuperMini
                      ┌─────────────────────┐
                GPIO0 ●──┬── 10k ── GND
                      │  └── R ──►|── GND      CH1
                GPIO1 ●──┬── 10k ── GND
                      │  └── R ──►|── GND      CH2
                GPIO3 ●──┬── 10k ── GND
                      │  └── R ──►|── GND      CH3
                GPIO4 ●──┬── 10k ── GND
                      │  └── R ──►|── GND      CH4
                      │
                GPIO5 ●── [BTN1] ── GND
                GPIO6 ●── [BTN2] ── GND
                GPIO7 ●── [BTN3] ── GND
               GPIO10 ●── [BTN4] ── GND
               GPIO20 ●── [MASTER] ── GND
                      │
                  GND ●──────────────────── общий GND
                      └─────────────────────┘
```

---

## Общее

### LED / PWM-выход (анти-глитч)

На каждый канал — **два** резистора:

```text
GPIO ──┬── 10 kΩ ── GND          pull-down (Reset / питание)
       │
       └── Rled ── LED/оптрон ── GND
```

Прошивка жмёт выходы в LOW как можно раньше, но **подтяжка в железе обязательна** для оптрона.

### Кнопки

```text
PIN -------- [ BUTTON ] -------- GND
отпущена = HIGH, нажата = LOW
```

### Master

- хотя бы один ON → ALL OFF;
- все OFF → ALL ON на сохранённой в RAM яркости;
- во время fade канала master замораживает яркость и блокирует ввод до отпускания кнопки канала.

После reboot все каналы OFF.

### Что проверяем

- CH1–CH4: short / hold / fade (первый hold после boot — DOWN; из OFF — 10% UP);
- master: any ON → ALL OFF, all OFF → ALL ON;
- master во время hold не портит сохранённую яркость;
- после Reset нет ложных вспышек (с pull-down);
- PWM 1 кГц, 10–100%; на ESP32 `100%` = full LEDC duty (1023 при 10-bit).
