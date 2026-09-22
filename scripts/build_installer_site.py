#!/usr/bin/env python3

import argparse
import hashlib
import json
import shutil
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
MANIFEST_TEMPLATE = ROOT / "installer" / "manifest.template.json"
INDEX_TEMPLATE = ROOT / "installer" / "index.html"


def build_site(version: str, firmware: Path, output: Path) -> None:
    if not firmware.is_file():
        raise FileNotFoundError(f"Firmware not found: {firmware}")

    output.mkdir(parents=True, exist_ok=True)
    firmware_dir = output / "firmware"
    firmware_dir.mkdir(parents=True, exist_ok=True)

    shutil.copyfile(INDEX_TEMPLATE, output / "index.html")

    firmware_output = firmware_dir / "esp-dimmer-full.bin"
    shutil.copyfile(firmware, firmware_output)

    manifest = json.loads(MANIFEST_TEMPLATE.read_text(encoding="utf-8"))
    manifest["version"] = version.removeprefix("v")
    (output / "manifest.json").write_text(
        json.dumps(manifest, ensure_ascii=False, indent=2) + "\n",
        encoding="utf-8",
    )

    (output / "version.txt").write_text(version + "\n", encoding="utf-8")

    digest = hashlib.sha256(firmware_output.read_bytes()).hexdigest()
    (firmware_dir / "SHA256SUMS.txt").write_text(
        f"{digest}  esp-dimmer-full.bin\n",
        encoding="utf-8",
    )


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Build ESP Dimmer Web Installer site")
    parser.add_argument("--version", required=True, help="Release tag, e.g. v0.1.0")
    parser.add_argument("--firmware", required=True, type=Path, help="Merged ESP32-C3 full.bin")
    parser.add_argument("--output", required=True, type=Path, help="Output site directory")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    build_site(args.version, args.firmware, args.output)

    for path in sorted(args.output.rglob("*")):
        if path.is_file():
            print(path.relative_to(args.output))


if __name__ == "__main__":
    main()
