#!/usr/bin/env python3

import hashlib
import json
import tempfile
from pathlib import Path

from build_installer_site import build_site


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

    fake_firmware = b"esp-dimmer-ci-test\x00\x01\x02"
    with tempfile.TemporaryDirectory() as tmp:
        tmp_dir = Path(tmp)
        firmware = tmp_dir / "input-full.bin"
        site = tmp_dir / "site"
        firmware.write_bytes(fake_firmware)

        build_site("v0.0.0-ci", firmware, site)

        generated_manifest = json.loads((site / "manifest.json").read_text(encoding="utf-8"))
        assert generated_manifest["version"] == "0.0.0-ci"
        assert (site / "index.html").read_text(encoding="utf-8") == html
        assert (site / "version.txt").read_text(encoding="utf-8") == "v0.0.0-ci\n"
        assert (site / "firmware" / "esp-dimmer-full.bin").read_bytes() == fake_firmware

        expected_digest = hashlib.sha256(fake_firmware).hexdigest()
        checksum = (site / "firmware" / "SHA256SUMS.txt").read_text(encoding="utf-8")
        assert checksum == f"{expected_digest}  esp-dimmer-full.bin\n"

    print("Web Installer configuration and site generation are valid")


if __name__ == "__main__":
    main()
