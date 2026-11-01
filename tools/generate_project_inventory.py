#!/usr/bin/env python3
"""Generate the phase handover inventory without modifying project inputs."""

from __future__ import annotations

import csv
import hashlib
import os
from datetime import datetime, timezone
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
OUTPUT = ROOT / "reports" / "PROJECT_FILE_INVENTORY.csv"
HEADER = [
    "file_path",
    "file_type",
    "size_bytes",
    "modified_utc",
    "sha256",
    "phase",
    "backup_included",
    "notes",
]
SKIP_DIRS = {".git", "__pycache__", ".pytest_cache", ".mypy_cache", ".ruff_cache"}


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as source:
        for chunk in iter(lambda: source.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def file_type(path: Path) -> str:
    suffix = path.suffix.lower()
    return {
        ".rom": "raw ROM",
        ".zip": "ZIP archive",
        ".gpr": "Ghidra project marker",
        ".prp": "Ghidra project metadata",
        ".gbf": "Ghidra database file",
        ".dat": "Ghidra project index",
        ".c": "C source",
        ".h": "C header",
        ".java": "Ghidra Java script",
        ".py": "Python script",
        ".md": "Markdown report",
        ".csv": "CSV data",
        ".log": "analysis log",
        ".txt": "text report",
    }.get(suffix, "file" if suffix else "extensionless Ghidra data")


def phase_for(relative: str) -> str:
    path = Path(relative)
    name = path.name
    if path.parts[0] == "sf2" or name == "sf2.zip":
        return "Phase 1 input"
    if name in {"sf2_analysis_report.md", "rom_identification.md", "rom_manifest.csv", "crc_report.txt"}:
        return "Phase 1"
    if name in {"mame_rom_definition.md", "memory_layout.md", "rom_pair_mapping.csv"}:
        return "Phase 2"
    if name in {"maincpu.bin", "rebuild_maincpu.py", "reconstruction_report.md"}:
        return "Phase 3"
    if name in {"SF2MainCPU.gpr", "InspectSf2Startup.java", "ghidra_import.md", "vector_analysis.md", "startup_analysis.md"} or "SF2MainCPU.rep" in path.parts:
        return "Phase 4"
    if name in {"function_catalog.md", "control_flow.md", "global_variables.md", "unresolved_functions.md", "InventorySf2Project.java", "DecompileSf2Candidates.java", "inventory.log", "inventory_final.log", "decompiled_functions.log", "candidate_decompilation.log", "candidate_decompilation_status.log"}:
        return "Phase 5"
    if name in {"player_control.md", "movement_system.md", "game_state_machine.md", "character_logic.md", "InspectSf2InputLiterals.java", "InspectSf2UiStrings.java", "input_literals.log", "input_literals_detail.log", "ui_string_refs.log", "phase6_decompiler.log", "phase6_decompiler_expanded.log"}:
        return "Phase 6"
    if name in {"collision_analysis.md", "combat_system.md", "damage_calculation.md", "hitbox_data.md", "phase7_candidate_decomp.log", "phase7_collision_helpers.log", "phase7_collision_dispatch.log", "phase7_collision_dispatch_final.log"}:
        return "Phase 7"
    if name in {"FINAL_ANALYSIS.md", "COMPLETE_FUNCTION_CATALOG.md", "MEMORY_MAP.md", "GAMEPLAY_ARCHITECTURE.md", "REMAINING_QUESTIONS.md", "ExportSf2FunctionCatalog.java", "final_reference_inventory.log"}:
        return "Phase 8"
    if path.parts[0] == "src" or name.startswith("source_") or name in {"ExportSf2SourceEvidence.java", "ExportSf2SourceMapping.java", "phase9_source_evidence.log", "phase9_source_evidence_final.log", "phase9_decompiler.log", "phase9_collision_decomp.log", "phase9_collision_boundary.log"}:
        return "Phase 9"
    if name.startswith("PROJECT_") or name == "generate_project_inventory.py":
        return "Project handover"
    if name.startswith("startup_listing") or name == "phase9_source_evidence.log":
        return "Ghidra evidence"
    return "Project support"


def main() -> None:
    expected_header = ",".join(HEADER) + "\n"
    if OUTPUT.exists() and OUTPUT.read_text(encoding="utf-8") != expected_header:
        raise SystemExit(f"Refusing to overwrite non-placeholder inventory: {OUTPUT}")

    records = []
    for base, directories, filenames in os.walk(ROOT):
        directories[:] = sorted(directory for directory in directories if directory not in SKIP_DIRS)
        for filename in filenames:
            path = Path(base) / filename
            relative = path.relative_to(ROOT).as_posix()
            if relative == OUTPUT.relative_to(ROOT).as_posix():
                continue
            stat = path.stat()
            original = relative.startswith("sf2/") or relative == "sf2.zip"
            note = (
                "Original ROM/archive; retained in place and excluded from backup."
                if original
                else "Included in external project backup."
            )
            records.append(
                {
                    "file_path": relative,
                    "file_type": file_type(path),
                    "size_bytes": stat.st_size,
                    "modified_utc": datetime.fromtimestamp(stat.st_mtime, timezone.utc)
                    .isoformat(timespec="seconds"),
                    "sha256": sha256(path),
                    "phase": phase_for(relative),
                    "backup_included": "no" if original else "yes",
                    "notes": note,
                }
            )

    records.sort(key=lambda row: row["file_path"])
    with OUTPUT.open("w", encoding="utf-8", newline="") as destination:
        writer = csv.DictWriter(destination, fieldnames=HEADER)
        writer.writeheader()
        writer.writerows(records)
    print(f"INVENTORY_PATH={OUTPUT.relative_to(ROOT)}")
    print(f"INVENTORY_FILES={len(records)}")
    print(f"INVENTORY_BYTES={sum(int(row['size_bytes']) for row in records)}")
    print(f"ORIGINALS_EXCLUDED_FROM_BACKUP={sum(row['backup_included'] == 'no' for row in records)}")


if __name__ == "__main__":
    main()
