# Схемы подключения контроллера

Две реализации контроллера: **ESP32-C3 — primary**, **NodeMCU ESP8266 — legacy / compatibility**.

Текущий этап управления реальной нагрузкой вынесен отдельно: [load-control-plan.md](load-control-plan.md).

- [Primary: ESP32-C3 SuperMini](#primary-esp32-c3-supermini)
- [Legacy: NodeMCU ESP8266](#legacy-nodemcu-esp8266)
- [Общее: LED / оптрон / master / проверка](#общее)

---

## Primary: ESP32-C3 SuperMini

Проверено на железе как контроллерный стенд.  
Кнопки не на boot-strap и не на native USB D+/D− (GPIO18/19).

Все кнопки — **active LOW → GND**, `INPUT_PULLUP`.  
PWM — **active HIGH**, 1 кГц, яркость 10–100%, boot = все OFF.  
`Serial` — native USB CDC (`ARDUINO_USB_CDC_ON_BOOT` в `platformio.ini`).

### Распиновка

| Назначение | LED / PWM | Кнопка |
|---|---|---|
| CH1 | GPIO0 | GPIO5 → GND |
| CH2 | GPIO1 | GPIO6 → GND |
| CH3 | GPIO3 | GPIO7 → GND |
| CH4 | GPIO4 | GPIO10 → GND |
| MASTER | — | GPIO20 → GND |

### Схема контроллерного стенда

![Контроллерный стенд ESP32-C3](assets/schematics/controller-stand.svg)

ASCII (compact):

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

Питание контроллера: USB-C либо штатный вход питания конкретной SuperMini согласно маркировке платы. Не соединять одновременно USB и внешний источник питания, пока power-path конкретной ревизии платы не проверен.

Для реальной нагрузки LED на PWM-выходе заменяется интерфейсом `PC817 -> BC548 -> DIM`:

![Схема канала управления нагрузкой](assets/schematics/load-control-channel.svg)

Четыре канала — отдельные интерфейсы, `DIM-` разных LDH не объединять:

![Четыре независимых DIM-интерфейса](assets/schematics/four-channel-overview.svg)

Подробности и bring-up: [load-control-plan.md](load-control-plan.md).

---

## Legacy: NodeMCU ESP8266

Compatibility / reference-стенд. Boot-strap на `D3`/`D4` приемлем: не держать BTN1/BTN2 при питании/Reset.

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

## Общее

### LED / PWM-выход контроллерного стенда

На каждый канал — **два** резистора:

```text
GPIO ──┬── 10 kΩ ── GND          pull-down (Reset / питание)
       │
       └── Rled ── LED ── GND
```

Прошивка жмёт выходы в LOW как можно раньше, но **hardware pull-down обязателен** и сохраняется при переходе к PC817.

При переходе к реальной нагрузке LED заменяется на `PC817 -> BC548 -> DIM` (схема выше и в [load-control-plan.md](load-control-plan.md)).

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

### Что проверяем на контроллерном стенде

- CH1–CH4: short / hold / fade (первый hold после boot — DOWN; из OFF — 10% UP);
- master: any ON → ALL OFF, all OFF → ALL ON;
- master во время hold не портит сохранённую яркость;
- после Reset нет ложных вспышек на GPIO (с pull-down);
- PWM 1 кГц, 10–100%; на ESP32 `100%` = full LEDC duty (1023 при 10-bit).

Для проверки реальной нагрузки используется отдельный acceptance checklist из [load-control-plan.md](load-control-plan.md).
