"""Principle schematic of one isolated PWM → DIM channel."""

from __future__ import annotations

import schemdraw
from schemdraw import elements as elm

schemdraw.use("svg")


def draw() -> schemdraw.Drawing:
    d = schemdraw.Drawing(show=False, bgcolor="white", unit=2.4, fontsize=11)

    d.add(elm.Dot().at((0.0, 3.0)).label("PWM GPIO", loc="left"))
    d.add(elm.Line().right().length(0.75))
    j_pwm = d.add(elm.Dot())

    r1 = d.add(elm.Resistor().right().label("R1 500 Ω", loc="top", ofst=(0, 0.12)))
    opto = d.add(elm.Optocoupler().anchor("anode").at(r1.end))
    d.add(elm.Label().at((opto.anode[0] + 0.15, opto.anode[1] + 0.7)).label("U1 PC817"))

    r2 = d.add(elm.Resistor().down().at(j_pwm.center))
    d.add(
        elm.Label()
        .at((j_pwm.center[0] - 1.25, (j_pwm.center[1] + r2.end[1]) / 2.0))
        .label("R2 10 kΩ")
    )
    d.add(elm.Line().down().length(0.55))
    gnd_dot = d.add(elm.Dot())
    d.add(elm.Ground().label("ESP GND", loc="right"))

    d.add(elm.Line().down().at(opto.cathode).toy(gnd_dot.center))
    d.add(elm.Line().left().tox(gnd_dot.center))

    iso_x = (opto.anode[0] + opto.collector[0]) / 2.0
    d.add(
        elm.Line()
        .at((iso_x, opto.collector[1] + 2.15))
        .to((iso_x, gnd_dot.center[1] - 0.25))
        .linestyle("--")
        .color("#546e7a")
    )
    d.add(
        elm.Label()
        .at((iso_x + 0.15, opto.collector[1] + 2.45))
        .label("изоляция PC817", fontsize=10)
    )

    d.add(elm.Line().right().at(opto.collector).length(1.9))
    base = d.add(elm.Dot())
    d.add(
        elm.Label()
        .at(((opto.collector[0] + base.center[0]) / 2.0, base.center[1] + 0.52))
        .label("BASE", fontsize=10)
    )

    q1 = d.add(elm.BjtNpn(circle=True).anchor("base").at(base.center))
    d.add(elm.Label().at((q1.collector[0] + 1.15, q1.base[1])).label("Q1 BC548"))
    d.add(elm.Label().at((base.center[0] - 0.45, base.center[1] - 0.38)).label("B", fontsize=9))
    d.add(elm.Label().at((q1.collector[0] + 0.42, q1.collector[1] + 0.28)).label("C", fontsize=9))
    d.add(elm.Label().at((q1.emitter[0] + 0.42, q1.emitter[1] - 0.28)).label("E", fontsize=9))

    d.add(elm.Line().up().at(q1.collector).length(1.7))
    dimp_tee = d.add(elm.Dot())
    d.add(elm.Line().right().length(1.45))
    d.add(elm.Dot(open=True).label("DIM+", loc="right"))

    r3_x = q1.base[0] - 1.05
    d.add(elm.Line().left().at(dimp_tee.center).tox(r3_x))
    d.add(elm.Dot())
    r3 = d.add(elm.Resistor().down())
    d.add(elm.Label().at((r3.center[0] + 0.9, r3.center[1])).label("R3 10 kΩ"))
    d.add(elm.Line().down().toy(base.center))
    d.add(elm.Line().right().tox(base.center))
    d.add(elm.Dot())

    d.add(elm.Line().down().at(q1.emitter).length(1.5))
    dime_tee = d.add(elm.Dot())
    d.add(elm.Line().right().length(1.45))
    d.add(elm.Dot(open=True).label("DIM−", loc="right"))

    d.add(elm.Line().down().at(opto.emitter).toy(dime_tee.center))
    d.add(elm.Line().right().tox(dime_tee.center))
    return d
