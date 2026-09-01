"""ESP32-C3 controller stand: test LEDs, pull-downs, and GND buttons."""

from __future__ import annotations

import schemdraw
from schemdraw import elements as elm

schemdraw.use("svg")

_PWM = (
    ("GPIO0", "CH1"),
    ("GPIO1", "CH2"),
    ("GPIO3", "CH3"),
    ("GPIO4", "CH4"),
)

_BUTTONS = (
    ("GPIO5", "BTN1"),
    ("GPIO6", "BTN2"),
    ("GPIO7", "BTN3"),
    ("GPIO10", "BTN4"),
    ("GPIO20", "MASTER"),
)

_BTN_X = 7.4


def _pwm_row(d: schemdraw.Drawing, y: float, gpio: str, ch: str) -> None:
    d.add(elm.Dot().at((0.0, y)).label(gpio, loc="left"))
    d.add(elm.Line().right().length(0.8))
    j = d.add(elm.Dot())

    d.add(elm.Line().down().length(0.45))
    rpd = d.add(elm.Resistor().down())
    d.add(elm.Label().at((rpd.center[0] - 0.9, rpd.center[1])).label("10 kΩ", fontsize=10))
    d.add(elm.Line().down().length(0.25))
    d.add(elm.Ground())

    d.add(elm.Line().right().at(j.center).length(0.45))
    rled = d.add(elm.Resistor().right())
    d.add(elm.Label().at((rled.center[0], y + 0.5)).label("Rled", fontsize=10))
    led = d.add(elm.LED().right())
    d.add(elm.Line().right().length(0.35))
    d.add(elm.Ground())
    d.add(elm.Label().at((led.center[0] + 1.4, y + 0.5)).label(ch, fontsize=10))


def _btn_row(d: schemdraw.Drawing, y: float, gpio: str, name: str) -> None:
    d.add(elm.Dot().at((_BTN_X, y)).label(gpio, loc="left"))
    d.add(elm.Line().right().length(0.45))
    d.add(elm.Button().right().label(name, loc="top", fontsize=10))
    d.add(elm.Line().right().length(0.4))
    d.add(elm.Ground())


def draw() -> schemdraw.Drawing:
    d = schemdraw.Drawing(show=False, bgcolor="white", fontsize=11, unit=1.0)

    d.add(elm.Label().at((6.2, 10.9)).label("\u00a0", fontsize=6))
    d.add(
        elm.Label()
        .at((6.2, 10.45))
        .label("Контроллерный стенд ESP32-C3 SuperMini", fontsize=13)
    )
    d.add(
        elm.Label()
        .at((6.2, 9.85))
        .label("тестовые LED · PWM active HIGH · кнопки active LOW → GND", fontsize=10)
    )

    d.add(elm.Label().at((2.2, 9.15)).label("PWM / LED", fontsize=10))
    d.add(elm.Label().at((9.6, 9.15)).label("кнопки", fontsize=10))

    y = 8.3
    for gpio, ch in _PWM:
        _pwm_row(d, y, gpio, ch)
        y -= 2.85

    y = 8.15
    for gpio, name in _BUTTONS:
        _btn_row(d, y, gpio, name)
        y -= 1.55

    d.add(
        elm.Label()
        .at((6.2, -2.4))
        .label("Rled ≈ 330–470 Ω · pull-down 10 kΩ обязателен · питание USB-C", fontsize=10)
    )
    return d
