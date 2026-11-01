# SF2 Reverse-Engineering-Labor

Statische Analyse und Dokumentation des MAME-Romsets **sf2eb** (*Street Fighter II - The World Warrior*, World 910214) mit Ghidra, plus einer eigenständigen Headless-C++20-Umsetzung als Prototyp.

## Projektstruktur

| Verzeichnis | Inhalt |
|---|---|
| `working/` | Ghidra-Analyse-Artefakte und Logs (Funktionskataloge, Dekompilierberichte) |
| `reports/` | 86 Dokumente + CSVs: Phasen 1–20, Funktions-/Coverage-Kataloge, Limitierungen |
| `src/` + `tests/` | Evidenzgebundene C-Rekonstruktion (3 C-Dateien, 2 Header, 1 Test) |
| `implementation/` | Eigenständige Headless-C++20/CMake-Umsetzung (Gameplay, Renderer, Audio, Input, Combat-Prototyp, 49 Tests) |
| `tools/` | Ghidra-Skripte und Python-Builder (z. B. `rebuild_maincpu.py`) |

*Ignoriert im Repo:* Original-ROMs (`sf2/`, `sf2.zip`), das abgeleitete ROM-Image (`working/maincpu.bin`), lokale Build-Outputs.

## Verifizierter Stand (Stand: 01.11.2026)

- CMake-Build + alle 5 Test-Executables bestanden, 0 Warnungen
- C-Sourcen mit `gcc -Wall -Wextra -Wpedantic` fehlerfrei
- `working/maincpu.bin` SHA-256 stimmt mit dokumentiertem Wert überein
- Ghidra: 353 Funktionsstarts, 11.356 Xrefs, vollständiger Funktionskatalog

## Wichtige Einschränkungen

- **Kein lauffähiges Spiel:** kein `main()`-Executable, keine GUI, keine Originaldaten (Roster/Moves/Sprites/Musik). Der Phasenstatus "COMPLETED" bezeichnet jeweils den Phasenumfang, nicht eine vollständige Rekonstruktion des Spiels.
- Von 353 Ghidra-Funktionen: 1 CONFIRMED-Präfix, 2 PROBABLE-Modelle, 350 ohne C-Implementierung.
- Combat-Prototyp = einfaches AABB-/Schadensmodell; Block, Knockback, Hitstun und echte Assets fehlen.
- Kein aktueller Gesamt-Backup: die vorliegenden Archive (`reverse-lab_backup_20261101.tar.gz`, `reverse-lab_handover_20261101.tar.gz`) sind Vor-Phase-10-Snapshots.

Details: [`reports/PROJECT_STATUS.md`](reports/PROJECT_STATUS.md), [`reports/KNOWN_LIMITATIONS.md`](reports/KNOWN_LIMITATIONS.md), [`reports/REMAINING_WORK.md`](reports/REMAINING_WORK.md).

## Build (Tests)

```sh
cmake -S implementation -B <build-dir> -DCMAKE_BUILD_TYPE=Debug
cmake --build <build-dir> --parallel 2
ctest --test-dir <build-dir> --output-on-failure
```

Es sind keine SDL-Abhängigkeiten nötig; der Core ist headless (Software-Renderer, WAV-/PPM-Platzhalter). Ein natives SDL3-Frontend ist geplant, aber nicht implementiert.

## Rechtliches

Original-ROMs und der abgeleitete ROM-Image sind urheberrechtlich geschützt (*Capcom*). Sie sind hier nur als privater Analyse-Input vorhanden und **nicht Teil dieses Repos** — nicht extrahieren oder in `implementation/assets/` übernehmen.

## Lizenz

Die neue C++20-Umsetzung folgt eigenständigem Design (kein Capcom-Code, keine ROM-Daten). *Lizenz: noch offen — bitte ergänzen.*
