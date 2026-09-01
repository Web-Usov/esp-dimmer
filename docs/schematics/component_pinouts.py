"""Physical pinouts for PC817 DIP-4 and onsemi BC548 TO-92.

Logical C/B/E and optocoupler terminals stay on load_control_channel.py.
This drawing is the mechanical view used before wiring."""

from __future__ import annotations

import schemdraw
from schemdraw import elements as elm
from schemdraw import flow

schemdraw.use("svg")


def _pc817(d: schemdraw.Drawing, west_x: float, y: float) -> None:
    body = d.add(flow.Box(w=3.6, h=2.5).label("PC817").at((west_x, y)))
    cx = (body.W[0] + body.E[0]) / 2.0
    top = body.N[1]

    d.add(elm.Label().at((cx, top + 1.55)).label("U1 PC817  DIP-4  вид сверху", fontsize=12))
    d.add(
        elm.Label()
        .at((cx, top + 1.05))
        .label("выемка сверху · pin 1 слева сверху", fontsize=10)
    )

    d.add(elm.Line().left().at((body.NW[0], body.NW[1] - 0.4)).length(0.55))
    d.add(elm.Dot())
    d.add(elm.Label().at((body.NW[0] - 1.7, body.NW[1] - 0.4)).label("1 Anode"))

    d.add(elm.Line().left().at((body.SW[0], body.SW[1] + 0.4)).length(0.55))
    d.add(elm.Dot())
    d.add(elm.Label().at((body.SW[0] - 1.85, body.SW[1] + 0.4)).label("2 Cathode"))

    d.add(elm.Line().right().at((body.SE[0], body.SE[1] + 0.4)).length(0.55))
    d.add(elm.Dot())
    d.add(elm.Label().at((body.SE[0] + 1.85, body.SE[1] + 0.4)).label("3 Emitter"))

    d.add(elm.Line().right().at((body.NE[0], body.NE[1] - 0.4)).length(0.55))
    d.add(elm.Dot())
    d.add(elm.Label().at((body.NE[0] + 1.95, body.NE[1] - 0.4)).label("4 Collector"))


def _bc548(d: schemdraw.Drawing, west_x: float, y: float) -> None:
    body = d.add(flow.Box(w=3.4, h=1.8).label("BC548").at((west_x, y + 0.35)))
    cx = (body.W[0] + body.E[0]) / 2.0
    top = body.N[1]
    bot = body.S[1]

    d.add(
        elm.Label()
        .at((cx, top + 1.55))
        .label("Q1 BC548 TO-92", fontsize=12)
    )
    d.add(
        elm.Label()
        .at((cx, top + 1.05))
        .label("плоская грань к себе · onsemi C B E вниз", fontsize=10)
    )

    pin_y = bot - 1.15
    span = body.E[0] - body.W[0]
    xs = (body.W[0] + span * 0.2, cx, body.E[0] - span * 0.2)
    names = ("1 C", "2 B", "3 E")
    for x, name in zip(xs, names):
        d.add(elm.Line().at((x, bot)).to((x, pin_y)))
        d.add(elm.Dot().at((x, pin_y)))
        d.add(elm.Label().at((x, pin_y - 0.45)).label(name, fontsize=11))


def draw() -> schemdraw.Drawing:
    d = schemdraw.Drawing(show=False, bgcolor="white", fontsize=11, unit=1.0)

    d.add(elm.Label().at((8.6, 8.15)).label("\u00a0", fontsize=6))
    d.add(
        elm.Label()
        .at((8.6, 7.7))
        .label("Физический pinout эталонного канала", fontsize=13)
    )
    d.add(
        elm.Label()
        .at((8.6, 7.15))
        .label("не путать с логическими C/B/E на принципиальной схеме", fontsize=10)
    )

    _pc817(d, 2.8, 3.6)
    _bc548(d, 11.4, 3.6)

    d.add(
        elm.Label()
        .at((8.6, 0.2))
        .label(
            "Перед сборкой прозвонить конкретную деталь. Клоны TO-92 могут быть E-B-C.",
            fontsize=10,
        )
    )
    return d
