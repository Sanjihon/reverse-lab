#!/usr/bin/env python3
"""Rebuild the sf2eb 68000 image from its verified ROM byte lanes."""

from __future__ import annotations

import hashlib
import os
import sys
import tempfile
import zlib
from dataclasses import dataclass
from pathlib import Path


OUTPUT_SIZE = 0x100000


@dataclass(frozen=True)
class RomLoad:
    pair: str
    filename: str
    offset: int
    size: int
    crc32: int
    lane: str


ROM_LOADS = (
    RomLoad("P1", "sf2e_30b.rom", 0x00000, 0x20000, 0x57BD7051, "even"),
    RomLoad("P1", "sf2e_37b.rom", 0x00001, 0x20000, 0x62691CDD, "odd"),
    RomLoad("P2", "sf2e_31b.rom", 0x40000, 0x20000, 0xA673143D, "even"),
    RomLoad("P2", "sf2e_38b.rom", 0x40001, 0x20000, 0x4C2CCEF7, "odd"),
    RomLoad("P3", "sf2_28b.rom", 0x80000, 0x20000, 0x4009955E, "even"),
    RomLoad("P3", "sf2_35b.rom", 0x80001, 0x20000, 0x8C1F3994, "odd"),
    RomLoad("P4", "sf2_29b.rom", 0xC0000, 0x20000, 0xBB4AF315, "even"),
    RomLoad("P4", "sf2_36b.rom", 0xC0001, 0x20000, 0xC02A13EB, "odd"),
)


class BuildError(Exception):
    pass


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def crc32(data: bytes) -> int:
    return zlib.crc32(data) & 0xFFFFFFFF


def main() -> int:
    root = Path(__file__).resolve().parents[1]
    rom_dir = root / "sf2"
    output_dir = root / "working"
    output_path = output_dir / "maincpu.bin"

    if not rom_dir.is_dir():
        raise BuildError(f"ROM directory not found: {rom_dir}")

    image = bytearray(OUTPUT_SIZE)
    occupied = bytearray(OUTPUT_SIZE)
    source_data: dict[str, bytes] = {}

    for load in ROM_LOADS:
        if Path(load.filename).name != load.filename:
            raise BuildError(f"Invalid ROM filename: {load.filename}")

        source_path = rom_dir / load.filename
        try:
            with source_path.open("rb") as source:
                data = source.read()
        except OSError as error:
            raise BuildError(f"Cannot read {source_path}: {error}") from error

        actual_crc = crc32(data)
        if len(data) != load.size:
            raise BuildError(
                f"{load.filename}: size {len(data):#x}, expected {load.size:#x}"
            )
        if actual_crc != load.crc32:
            raise BuildError(
                f"{load.filename}: CRC32 {actual_crc:08x}, "
                f"expected {load.crc32:08x}"
            )
        if load.offset & 1 != (load.lane == "odd"):
            raise BuildError(f"{load.filename}: offset does not match {load.lane} lane")

        first_address = load.offset
        last_address = load.offset + 2 * (load.size - 1)
        stop_address = last_address + 1
        if first_address < 0 or stop_address > OUTPUT_SIZE:
            raise BuildError(
                f"{load.filename}: mapped range {first_address:#x}-{last_address:#x} "
                f"exceeds output size {OUTPUT_SIZE:#x}"
            )

        lane_slice = slice(first_address, stop_address, 2)
        if any(occupied[lane_slice]):
            raise BuildError(f"{load.filename}: mapped bytes overlap an earlier ROM")
        image[lane_slice] = data
        occupied[lane_slice] = b"\x01" * load.size
        source_data[load.filename] = data

    pairs: dict[str, list[RomLoad]] = {}
    for load in ROM_LOADS:
        pairs.setdefault(load.pair, []).append(load)

    for pair_name, pair_loads in pairs.items():
        by_lane = {load.lane: load for load in pair_loads}
        if len(pair_loads) != 2 or set(by_lane) != {"even", "odd"}:
            raise BuildError(f"{pair_name}: expected exactly one even and one odd ROM")
        even = by_lane["even"]
        odd = by_lane["odd"]
        if even.size != odd.size or odd.offset != even.offset + 1:
            raise BuildError(f"{pair_name}: even/odd ROMs do not form a matched pair")

    if occupied.count(1) != OUTPUT_SIZE:
        missing = occupied.count(0)
        raise BuildError(f"mapped image has {missing} uncovered byte addresses")

    for load in ROM_LOADS:
        stop_address = load.offset + 2 * load.size
        if bytes(image[load.offset:stop_address:2]) != source_data[load.filename]:
            raise BuildError(f"{load.filename}: reconstructed lane differs from source")

    rom_paths = [(rom_dir / load.filename).resolve() for load in ROM_LOADS]
    try:
        output_dir.mkdir(parents=True, exist_ok=True)
        resolved_rom_dir = rom_dir.resolve(strict=True)
        resolved_output_dir = output_dir.resolve(strict=True)
    except ValueError:
        raise BuildError("Cannot compare output directory with original ROM directory")
    except OSError as error:
        raise BuildError(f"Cannot prepare output directory: {error}") from error

    try:
        resolved_output_dir.relative_to(resolved_rom_dir)
    except ValueError:
        pass
    else:
        raise BuildError("working directory resolves inside the original ROM directory")

    resolved_output = output_path.resolve(strict=False)
    if resolved_output in rom_paths:
        raise BuildError("output path resolves to an original ROM file")

    temp_path: Path | None = None
    try:
        with tempfile.NamedTemporaryFile(
            mode="wb", dir=output_dir, prefix=".maincpu.", suffix=".tmp", delete=False
        ) as temporary:
            temp_path = Path(temporary.name)
            temporary.write(image)
            temporary.flush()
            os.fsync(temporary.fileno())

        with temp_path.open("rb") as rebuilt:
            written_data = rebuilt.read()
        if written_data != image:
            raise BuildError("temporary output differs from reconstructed image")

        os.replace(temp_path, output_path)
        temp_path = None

        with output_path.open("rb") as rebuilt:
            final_data = rebuilt.read()
        if len(final_data) != OUTPUT_SIZE or final_data != image:
            raise BuildError("final output does not match the reconstructed image")
    except OSError as error:
        raise BuildError(f"Cannot write {output_path}: {error}") from error
    finally:
        if temp_path is not None:
            try:
                temp_path.unlink()
            except FileNotFoundError:
                pass

    print(f"Output: {output_path.relative_to(root)}")
    print(f"Size: {len(final_data)} bytes (0x{len(final_data):x})")
    print(f"SHA-256: {sha256(final_data)}")
    print(f"CRC32: {crc32(final_data):08x}")
    print(f"Coverage: {occupied.count(1)}/{OUTPUT_SIZE} byte addresses; no gaps or overlaps")
    print("ROM checks (source bytes, mapped address span):")
    for load in ROM_LOADS:
        data = source_data[load.filename]
        last_address = load.offset + 2 * (load.size - 1)
        print(
            f"  {load.pair} {load.lane:4} {load.filename}: "
            f"CRC32={crc32(data):08x} first={data[0]:02x} last={data[-1]:02x} "
            f"addresses={load.offset:#07x}-{last_address:#07x} bytes={len(data)}"
        )
    print("Pair range boundary bytes (reconstructed image):")
    for pair_name, pair_loads in pairs.items():
        first = min(load.offset for load in pair_loads)
        end = max(load.offset + 2 * (load.size - 1) + 1 for load in pair_loads)
        print(
            f"  {pair_name}: {first:#08x}-{end - 1:#08x} "
            f"first={final_data[first]:02x} last={final_data[end - 1]:02x}"
        )
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except BuildError as error:
        print(f"ERROR: {error}", file=sys.stderr)
        raise SystemExit(1) from error