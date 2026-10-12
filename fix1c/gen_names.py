# -*- coding: utf-8 -*-
"""src/names_ru.inc (UTF-8) -> Out/gen/names_ru_cp1251.inc (CP1251).

1C compares method names as CP1251 bytes, and VC6 puts string bytes into the DLL as is, so the
compiler must see the names already in CP1251. Run by build.cmd before nmake.
"""
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parent
SRC = ROOT / "src" / "names_ru.inc"
DST = ROOT / "Out" / "gen" / "names_ru_cp1251.inc"


def main() -> int:
    text = SRC.read_text(encoding="utf-8")
    try:
        data = text.encode("cp1251")
    except UnicodeEncodeError as error:
        print(f"names_ru.inc: symbol outside CP1251: {error}")
        return 1
    DST.parent.mkdir(parents=True, exist_ok=True)
    DST.write_bytes(data.replace(b"\r\n", b"\n").replace(b"\n", b"\r\n"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
