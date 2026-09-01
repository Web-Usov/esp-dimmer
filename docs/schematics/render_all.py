#!/usr/bin/env python3
"""Render documentation schematics to SVG. Paths are relative to this file."""

from __future__ import annotations

import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
if str(HERE) not in sys.path:
    sys.path.insert(0, str(HERE))

import schemdraw

schemdraw.use("svg")

from component_pinouts import draw as draw_component_pinouts
from controller_stand import draw as draw_controller_stand
from four_channel_overview import draw as draw_four_channel
from load_control_channel import draw as draw_load_control
from system_overview import draw as draw_system

OUT_DIR = HERE.parent / "assets" / "schematics"

SPECS = (
    ("system-overview.svg", draw_system),
    ("controller-stand.svg", draw_controller_stand),
    ("load-control-channel.svg", draw_load_control),
    ("four-channel-overview.svg", draw_four_channel),
    ("component-pinouts.svg", draw_component_pinouts),
)


def main() -> int:
    OUT_DIR.mkdir(parents=True, exist_ok=True)
    for name, draw in SPECS:
        path = OUT_DIR / name
        try:
            drawing = draw()
            drawing.save(str(path))
        except Exception as exc:
            print(f"error rendering {name}: {exc}", file=sys.stderr)
            return 1
        print(f"wrote {path}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
