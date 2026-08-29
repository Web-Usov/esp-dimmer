# Схема тестового стенда

**NodeMCU ESP8266**: 4 канала + 1 master-кнопка.

Все кнопки — **active LOW → GND**, `INPUT_PULLUP` (без `3V3`).

## Распиновка

| Назначение | LED / PWM | Кнопка |
|---|---|---|
| CH1 | `D1` | `D3` → GND |
| CH2 | `D2` | `D4` → GND |
| CH3 | `D5` | `D7` → GND |
| CH4 | `D8` | `D6` → GND |
| MASTER | — | `D9` / `RX` → GND |

`D0` свободен. Serial — только вывод через `TX`.

## Схема

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

## LED / PWM-выход

На каждый канал — **два** резистора:

```text
Dx ──┬── 10 kΩ ── GND          pull-down (анти-глитч при Reset/питании)
     │
     └── Rled ── LED ── GND    токоограничение
```

Без pull-down GPIO при Reset может кратко «всплыть» и вспыхнуть LED (на стенде заметно на ch3/`D5`). Прошивка жмёт выходы в LOW как можно раньше, но **подтяжка в железе обязательна** для оптрона/нагрузки.

### Задел под оптрон (HIGH = вкл)

```text
Dx ──┬── 10 kΩ ── GND
     └── Rseries ── анод LED оптрона ── катод ── GND
```

## Кнопки

Все пять одинаково:

```text
PIN -------- [ BUTTON ] -------- GND
отпущена = HIGH, нажата = LOW
```

## Master

Одно срабатывание на нажатие (есть debounce, нет fade):

- есть хотя бы один ON → выключить все 4;
- все OFF → включить все 4 на сохранённой в RAM яркости.

После reboot все каналы OFF.

## Замечания

1. `D3` / `D4` — boot-strap: не держите BTN1/BTN2 при включении или Reset.
2. `D8` — LED (при OFF = LOW), не кнопка: на GPIO15 у NodeMCU сильный board pull-down.
3. `D9`/`RX` — master; ввод в Serial с ПК недоступен.
4. `D0` не используем для кнопок: нет обычного `INPUT_PULLUP`.

## Что проверяем

- CH1–CH4: short press / hold / fade;
- master: any ON → ALL OFF, all OFF → ALL ON;
- яркость после master OFF/ON сохраняется;
- после Reset нет ложных вспышек LED (с pull-down);
- PWM 1 кГц, яркость 10–100%.
