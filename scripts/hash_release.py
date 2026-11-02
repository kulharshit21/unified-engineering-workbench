"""Calculate a reproducible SHA-256 sidecar for a packaged Windows ZIP."""
from __future__ import annotations

import hashlib
import sys
from pathlib import Path


def main() -> int:
    if len(sys.argv) != 3:
        print("Usage: hash_release.py ARCHIVE.zip ARCHIVE.sha256", file=sys.stderr)
        return 2

    archive = Path(sys.argv[1])
    sidecar = Path(sys.argv[2])
    if not archive.is_file():
        print(f"[ERROR] Archive not found: {archive}", file=sys.stderr)
        return 1

    digest = hashlib.sha256()
    try:
        with archive.open("rb") as stream:
            for block in iter(lambda: stream.read(1024 * 1024), b""):
                digest.update(block)
        sidecar.write_text(f"{digest.hexdigest().upper()}  {archive.name}\n", encoding="ascii")
    except OSError as exc:
        print(f"[ERROR] Could not write SHA-256 sidecar: {exc}", file=sys.stderr)
        return 1
    print(f"[PASS] SHA-256: {digest.hexdigest().upper()}  {archive.name}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
