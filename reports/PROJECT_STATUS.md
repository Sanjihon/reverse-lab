# Projektstatus Phasen 1-20

Stand: 2026-11-01. Alle Phasen 1-20 wurden bearbeitet; `COMPLETED` bezeichnet den jeweiligen Phasenumfang, nicht ein vollstaendig rekonstruiertes oder originalgetreues Spiel.

| Phase | Status | Nachweis / Begruendung |
|---:|---|---|
| 1 | COMPLETED | 23 lokale ROMs identifiziert und CRC32 gegen MAME `sf2eb` abgeglichen; `reports/rom_identification.md`, `reports/rom_manifest.csv`, `reports/crc_report.txt`. 8 MAME-PLDs fehlen lokal. |
| 2 | COMPLETED | Vollstaendige `ROM_START(sf2eb)`-Definition und reale LOAD/CONTINUE-Offsets untersucht; `reports/mame_rom_definition.md`, `reports/memory_layout.md`, `reports/rom_pair_mapping.csv`. |
| 3 | COMPLETED | `working/maincpu.bin` aus acht Even/Odd-ROMs gebaut und validiert; 1 MiB, SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`, CRC32 `94d86e85`; Builder `tools/rebuild_maincpu.py`; Bericht `reports/reconstruction_report.md`. |
| 4 | COMPLETED | Ghidra 12.1.4-Projekt `working/SF2MainCPU.gpr`/`.rep` angelegt; Raw Binary, m68k, `68000:BE:32:default`, Big Endian, Base `0x0`; Vektoren und Reset-Instruktionen untersucht; vier Import-/Startup-Berichte vorhanden. |
| 5 | COMPLETED | Gespeichertes Ghidra-Projekt inventarisiert/dekompiliert; `reports/function_catalog.md`, `reports/control_flow.md`, `reports/global_variables.md`, `reports/unresolved_functions.md`. Spaeterer Phase-8-Audit aktualisiert den Funktionsbestand auf 353. |
| 6 | COMPLETED | MAME-P1/P2-Portbits dokumentiert; Ghidra-Input-Literal- und UI-String-Xref-Suchen ausgefuehrt; keine ROM-Input-Leseroutine oder String-Xrefs nachgewiesen; vier Berichte in `reports/player_control.md` usw. |
| 7 | COMPLETED | Collider-/Overlap-Kandidat `FUN_0007e25e`/`FUN_0007e486` und relative Dispatch-Tabelle untersucht; `reports/collision_analysis.md`, `reports/combat_system.md`, `reports/damage_calculation.md`, `reports/hitbox_data.md`. |
| 8 | COMPLETED | Abschlussberichte, vollstaendiger Funktionskatalog und aktueller ReferenceManager-Xref-Audit erstellt; `reports/FINAL_ANALYSIS.md`, `reports/COMPLETE_FUNCTION_CATALOG.md`, `reports/MEMORY_MAP.md`, `reports/GAMEPLAY_ARCHITECTURE.md`, `reports/REMAINING_QUESTIONS.md`; 353 Funktionen/11.356 Xrefs im aktuellen Projekt. |
| 9 | COMPLETED | Partielle, evidenzgebundene C-Rekonstruktion; 3 C-Dateien, 2 Header, 353-zeilige Ghidra-Mapping-CSV; 1 CONFIRMED Prefix, 2 PROBABLE Entry-Zuordnungen, 350 UNKNOWN. Syntaxcheck bestanden; ROM/Image-Hashes unveraendert. |
| 10 | COMPLETED | Coverage-Abgleich 353 Ghidra-Starts; 1 partieller CONFIRMED Prefix, 2 PROBABLE Modellstarts, 350 ohne C-Implementierung; Status-/Coverage-/Resume-Berichte erstellt. |
| 11 | COMPLETED | Zwei C-Modelle/3 Ghidra-Starts statisch verglichen; 30 C-Assertions; 0 dynamische Originaltests; Phase-11-Berichte und Input-/Resetkorrektur dokumentiert. |
| 12 | COMPLETED | Eigenstaendige C++20/CMake/SDL3-Architektur, Modulzuordnung, Roadmap und leere Modulstruktur angelegt; keine Spielimplementierung in der Phase. |
| 13 | COMPLETED | Headless Gameplay-Core, Spieler-/Bewegungs-/Runden-/State-Modell; 14 Gameplay-Testfaelle. Prototypewerte als neu gekennzeichnet. |
| 14 | COMPLETED | Headless Software-Renderer, P6-PPM/Platzhalter, virtuelle Skalierung und Animationscontroller; 7 Grafiktests; kein natives SDL-Fenster. |
| 15 | COMPLETED | PCM16-WAV-Ressourcen, Mixer, Cue-/Musikrouting, Null-/WAV-Ausgang; 8 Audio-Tests; kein Live-Device. |
| 16 | COMPLETED | Normalisierter Keyboard/Gamepad/Joystick-Input, P1/P2-Bindings, Buffer und Motion-Recognizer; 11 Inputtests; kein SDL-Pollingadapter. |
| 17 | COMPLETED | GameApplication-Tick, Collision-/Combat-Prototype, Events und Buildintegration; 9 Integrationstests; kein standalone `main()`-Executable. |
| 18 | COMPLETED | 47 automatisierte Testfaelle dokumentiert; 10.000-Tick-Soak und Funktions-/Charakter-/Gameplay-Berichte. |
| 19 | COMPLETED | Zwei Input-/App-Lifecyclefehler behoben; Debug/Release/Sanitizer-Regressionen bestanden; Performance ohne unbelegte Optimierung gemessen. |
| 20 | COMPLETED | Abschlussberichte, Phasenstatus, Architektur-/Build-/Evidenztrennung und offene Arbeiten konsolidiert; Integrity-Audit erneut bestanden. |

## Aktueller Projektzustand

- MAME-Set: `sf2eb`, World 910214.
- Rekonstruiertes Raw-Image: `working/maincpu.bin`, 1.048.576 Bytes, Image Base `0x0`.
- Ghidra: 12.1.4, Motorola m68k, `68000:BE:32:default`, Compiler Spec `default`.
- Ghidra-Projekt: `working/SF2MainCPU.gpr` plus `working/SF2MainCPU.rep/`.
- Source-Prototyp: `src/` enthaelt `common/types.h`, `memory/memory_map.h`, `system/startup.c`, `system/interrupts.c`, `game/collision.c`.
- Eigenstaendige Headless-C++20-Implementierung mit generatorneutralen CMake Debug-/Release-Presets, fuenf Testzielen und 49 Testfaellen. Es gibt noch kein natives SDL3-Frontend und kein distributable Spiel-Executable.
- Phase-5/6-Funktionszahlen von 345 sind historische Snapshots; aktuell enthaelt das Ghidra-Projekt 353 Funktionsstarts nach Anlage von acht Kollisions-Dispatchzielen.

## Integritaet

Letzter bestaetigter Read-only-Check: 23/23 vorhandene ROM-SHA-256-Werte stimmen mit `reports/rom_manifest.csv`; `working/maincpu.bin` hat SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`. Phase-9-QA hat beide Werte erneut bestaetigt. Es wurden keine Original-ROMs veraendert.

## Historischer Sicherungsstand

- `/home/user/reverse-lab_handover_20261101.tar.gz` und `/home/user/reverse-lab_backup_20261101.tar.gz` sind historische Vor-Phase-10-Snapshots, keine Backups des Phase-20-Endstands.
- Sie enthalten den damaligen, geprueften Stand; Phasen 10-20 und deren Tests/Berichte sind nicht enthalten. Ein neues finales externes Backup wurde in Phase 20 nicht erstellt.
- Die damalige Sicherung liess `sf2/` und `sf2.zip` absichtlich aus; fuer den aktuellen Endstand gilt die erneute Integritaetspruefung in `reports/FINAL_TEST_REPORT.md`.
