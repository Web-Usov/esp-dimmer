"""Four independent PWM → DIM interfaces. DIM− of different LDH are not commoned."""

from __future__ import annotations

import schemdraw
from schemdraw import elements as elm
from schemdraw import flow

schemdraw.use("svg")

_CHANNELS = (
    ("GPIO0", "CH1", "LDH #1"),
    ("GPIO1", "CH2", "LDH #2"),
    ("GPIO3", "CH3", "LDH #3"),
    ("GPIO4", "CH4", "LDH #4"),
)


def _row(d: schemdraw.Drawing, y: float, gpio: str, ch: str, ldh: str) -> None:
    gpio_dot = d.add(elm.Dot().at((0.0, y)).label(gpio, loc="left"))
    iface = d.add(
        flow.Box(w=4.4, h=1.45)
        .label(f"{ch}\nPC817 + BC548 + R")
        .at((4.4, y))
    )
    ldh_box = d.add(flow.Box(w=2.4, h=1.45).label(ldh).at((10.8, y)))

    d.add(flow.Arrow().at(gpio_dot.center).to(iface.W))
    d.add(elm.Line().at(iface.ENE).to(ldh_box.WNW))
    d.add(
        elm.Label()
        .at(((iface.ENE[0] + ldh_box.WNW[0]) / 2.0, iface.ENE[1] + 0.28))
        .label("DIM+", fontsize=9)
    )
    d.add(elm.Line().at(iface.ESE).to(ldh_box.WSW))
    d.add(
        elm.Label()
        .at(((iface.ESE[0] + ldh_box.WSW[0]) / 2.0, iface.ESE[1] - 0.32))
        .label("DIM−", fontsize=9)
    )


def draw() -> schemdraw.Drawing:
    d = schemdraw.Drawing(show=False, bgcolor="white", fontsize=11, unit=1.0)
    d.add(
        elm.Label()
        .at((7.0, 9.2))
        .label("Четыре одинаковых интерфейса PC817 + BC548 + резисторы", fontsize=12)
    )

    y = 7.4
    for gpio, ch, ldh in _CHANNELS:
        _row(d, y, gpio, ch, ldh)
        y -= 2.15

    d.add(
        elm.Label()
        .at((7.0, y + 0.55))
        .label("DIM− каждого LDH свой: линии DIM− разных каналов не объединять.", fontsize=10)
    )
    return d
