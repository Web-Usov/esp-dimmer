#!/usr/bin/env python3

import json
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST = ROOT / "installer" / "manifest.template.json"
INDEX = ROOT / "installer" / "index.html"


def main() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    html = INDEX.read_text(encoding="utf-8")

    assert manifest["name"] == "ESP Dimmer"
    assert manifest["version"] == "__VERSION__"
    assert manifest["new_install_prompt_erase"] is False

    builds = manifest["builds"]
    assert len(builds) == 1
    assert builds[0]["chipFamily"] == "ESP32-C3"

    parts = builds[0]["parts"]
    assert parts == [{"path": "firmware/esp-dimmer-full.bin", "offset": 0}]

    assert 'manifest="manifest.json"' in html
    assert "esp-web-tools@10/dist/web/install-button.js?module" in html
    assert "<esp-web-install-button" in html

    print("Web Installer configuration is valid")


if __name__ == "__main__":
    main()
