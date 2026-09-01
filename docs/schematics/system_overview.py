"""Architectural system overview: mains, 12 V, 5 V, LED power, PWM/DIM."""

from __future__ import annotations

import schemdraw
from schemdraw import elements as elm
from schemdraw import flow

schemdraw.use("svg")


def draw() -> schemdraw.Drawing:
    d = schemdraw.Drawing(show=False, bgcolor="white", fontsize=11, unit=1.0)

    d.add(
        elm.Label()
        .at((8.6, 8.35))
        .label("Архитектура питания и управления", fontsize=13)
    )

    d.add(elm.Label().at((1.4, 7.35)).label("сеть 230 VAC", fontsize=10, color="#b71c1c"))
    d.add(elm.Label().at((7.6, 7.35)).label("шина 12 V", fontsize=10, color="#1565c0"))
    d.add(elm.Label().at((13.8, 7.35)).label("питание LED", fontsize=10, color="#6a1b9a"))

    mains = d.add(flow.Box(w=2.6, h=1.15).label("230 VAC").at((1.4, 6.2)))
    lpv = d.add(flow.Box(w=3.2, h=1.15).label("LPV-35-12").at((5.6, 6.2)))
    ldh = d.add(flow.Box(w=3.6, h=1.35).label("LDH-25-350W").at((11.4, 6.2)))
    led = d.add(flow.Box(w=2.8, h=1.15).label("LED\n~36 V / 350 mA").at((16.0, 6.2)))

    d.add(flow.Arrow().at(mains.E).to(lpv.W))
    d.add(flow.Arrow().at(lpv.E).to(ldh.W).label("12 V", loc="top", fontsize=10))
    d.add(flow.Arrow().at(ldh.E).to(led.W))

    d.add(elm.Label().at((5.6, 4.55)).label("контроллер 5 V", fontsize=10, color="#2e7d32"))
    dcdc = d.add(flow.Box(w=3.2, h=1.2).label("DC/DC buck\n5 V").at((5.6, 3.35)))
    esp = d.add(flow.Box(w=2.8, h=1.2).label("ESP32-C3").at((10.0, 3.35)))

    d.add(flow.Arrow().at(lpv.S).to(dcdc.N))
    d.add(flow.Arrow().at(dcdc.E).to(esp.W).label("5 V", loc="top", fontsize=10))

    opto = d.add(flow.Box(w=2.8, h=1.15).label("PC817").at((10.0, 0.15)))
    bjt = d.add(flow.Box(w=2.6, h=1.15).label("BC548").at((13.8, 0.15)))
    d.add(elm.Label().at((11.9, -0.95)).label("PWM / DIM", fontsize=10, color="#ef6c00"))

    d.add(flow.Arrow().at(esp.S).to(opto.N))
    d.add(
        elm.Label()
        .at((esp.S[0] - 0.7, (esp.S[1] + opto.N[1]) / 2.0))
        .label("PWM", fontsize=10)
    )
    d.add(flow.Arrow().at(opto.E).to(bjt.W))
    d.add(elm.Wire("-|", arrow="->").at(bjt.N).to(ldh.S))
    d.add(
        elm.Label()
        .at((ldh.S[0] + 1.1, (bjt.N[1] + ldh.S[1]) / 2.0))
        .label("DIM+/DIM−", fontsize=10)
    )

    return d
