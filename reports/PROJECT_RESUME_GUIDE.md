# Wiederaufnahmeleitfaden

Stand: 2026-10-31. Zweck: Ein anderer Agent kann den belegten Zustand fortsetzen, ohne abgeschlossene Pakete erneut auszufuehren.

## Projektziel und Architektur

Partielle Reverse-Engineering- und Source-Rekonstruktion des MAME-Sets `sf2eb` (Street Fighter II: The World Warrior, World 910214). Board/CPU-Grundlage: CPS-1, Motorola 68000, Big Endian, 16-Bit-Datenbus. Ghidra importiert den headerlosen 1-MiB-Programm-Blob bei CPU-Adresse `0x0` mit Language `68000:BE:32:default`. MAMEs `main_map` weist ROM `0x000000-0x3fffff` zu; der gelieferte Programm-Blob belegt `0x000000-0x0fffff`.

Das Ziel ist keine vollstaendige Emulator-/Arcade-Portierung. Bisherige C-Dateien sind partielle, zu Ghidra-Adressen gemappte Modelle und kein originaler Capcom-Quellcode.

## Verzeichnisse und wichtige Pfade

- `sf2/`: 23 Original-ROM-Dateien. Ausschliesslich lesend behandeln.
- `sf2.zip`: Urspruengliches ROM-Archiv; nicht in das Sicherungsarchiv dupliziert.
- `working/maincpu.bin`: rekonstruierter 68000-Blob; **niemals ueberschreiben**.
- `working/SF2MainCPU.gpr`, `working/SF2MainCPU.rep/`: persistentes Headless-Ghidra-Projekt und Datenbank.
- `working/final_reference_inventory.log`: aktuelle komplette Funktions-/ReferenceManager-Inventur (353 Funktionen, 11.356 Xrefs).
- `working/phase9_source_evidence.log`, `working/phase9_decompiler.log`: ausgewählte Assemblerspannen, Xrefs und Decompilerstatus fuer die C-Rekonstruktion.
- `reports/`: Phasenberichte, ROM-Manifest und Resume-Artefakte. Keine bisherigen Berichte ueberschreiben.
- `src/`: partielle C-Rekonstruktion, siehe unten.
- `tools/`: ROM-Builder, Ghidra-Headless-Skripte, Inventar-/Mappingexporter.

## Phasen 1-20 abgeschlossen

Alle zwanzig geplanten Projektphasen wurden bearbeitet. Das bedeutet nicht, dass die Originalsoftware vollstaendig rekonstruiert oder ein originalgetreues Spiel erstellt wurde. Die Rekonstruktion bleibt partiell; `implementation/` ist eine eigenstaendige Headless-Prototypimplementierung mit dokumentierten neuen Regeln.

Aktueller Ghidra-Stand: 353 Funktionsstarts und 11.356 ReferenceManager-Xrefs. Phase 9 rekonstruierte einen bestaetigten partiellen Reset-Prefix und ein wahrscheinliches Collision-Hostmodell fuer zwei Ghidra-Starts. Phase 10-11 prueften Coverage und statische Uebereinstimmung. Phase 12-19 fuehrten neue Spiel-, Grafik-, Audio-, Input-, Combat-, Build- und Testmodule ein. Phase 20 fasst Evidenz, Grenzen und offene Arbeit zusammen.

Endgueltige Reports: `reports/FINAL_PROJECT_REPORT.md`, `reports/FINAL_RECONSTRUCTION_REPORT.md`, `reports/FINAL_IMPLEMENTATION_REPORT.md`, `reports/FINAL_TEST_REPORT.md`, `reports/PROJECT_ARCHITECTURE.md`, `reports/BUILD_AND_INSTALLATION.md`, `reports/KNOWN_LIMITATIONS.md` und `reports/REMAINING_WORK.md`.

## Vorhandene C-Dateien

- `src/common/types.h`: fixed-width Typaliases und `_Static_assert`s.
- `src/memory/memory_map.h`: MAME-verifizierte Adressen/Portmasken, keine Host-Pointermap.
- `src/system/startup.c`: CONFIRMED partieller Reset-Seiteneffekt-Prefix. Die LEA bei `0x41e` verweist auf `0x426`; `JMP (A4)` bei `0x750` ist statisch zum zweiten LEA-Einstieg `0x426` rueckfuehrbar, danach verzweigt `0x42a` nach `0x7cc`. Der volle Resetpfad bleibt ohne dynamische Validierung.
- `src/system/interrupts.c`: beobachtete Vektorindex-/Zieltabelle; keine Handleremulation.
- `src/game/collision.c`: PROBABLE Host-Modell des Overlap-Pfads. `FUN_0007e486` ist ein 12-Byte-Ghidra-Entryfragment; `FUN_0007e492` ist der 156-Byte-Comparator. Rueckgabesignatur/Parameter sind ein Host-Adapter und keine wiederhergestellte 68000-ABI.

Die unabhaengige C++20-Implementierung liegt getrennt unter `implementation/`: GameSession/Loop, zwei generische Trainingsprofile, Input, Collision/Combat-Prototyp, Animation, Software-Renderer, PCM-Audio und Application-Orchestrierung. Sie hat kein natives SDL-Frontend, kein `main()`-Executable und keine SF2-Roster-/Move-Daten.

## Integritaet und vorab notwendige Pruefungen

Erwartete SHA-256:

- `working/maincpu.bin`: `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`.
- 23 vorhandene Original-ROM-SHA-256-Werte: `reports/rom_manifest.csv` (read-only vergleichen).

Vor jeder Fortsetzung:

1. `reports/PROJECT_STATUS.md` und dieses Dokument lesen.
2. Das externe Archiv `/home/user/reverse-lab_handover_20261101.tar.gz` ist ein Snapshot vor Phase 10 und enthaelt nicht den heutigen Phase-20-Endstand. Es ist kein aktueller Restorepunkt.
3. `sha256sum working/maincpu.bin` und ROM-Hashes gegen Manifest pruefen.
4. `test -f working/SF2MainCPU.gpr` sowie `test -f working/SF2MainCPU.rep/project.prp` ausfuehren; vor GUI/Headless-Schreiblaeufen sicherstellen, dass kein anderer Ghidra-Prozess dasselbe Projekt offen hat.
5. Ghidra 12.1.4, `support/analyzeHeadless`, GCC/`cc` und Python 3 auf Verfuegbarkeit pruefen; keine neuen Dependencies installieren.
6. Bestehende Berichte nicht ueberschreiben; neue Phase-Artefakte mit neuem Pfad anlegen oder gezielte Diffs verwenden.

## Bekannte offene Probleme

- Reset-Folgepfad: statisch `0x750 -> 0x426 -> 0x42a -> 0x7cc`; vollstaendige Reset-/Main-Loop-Erreichbarkeit ist nicht dynamisch geprueft.
- Ghidra-Standard-Autoanalyse hatte 488 nichtfatale PCode-Warnungen; grosse Funktionen und Switch-Bereiche koennen unzuverlaessige Grenzen haben.
- Keine sichere MAME-IN1-Leseroutine, keine Character-/Attack-/Damage-/HP-Zuordnung.
- `FUN_0007e25e`/`FUN_0007e486` ist generische Overlaplogik; Angriffshitbox vs. Hurtbox bleibt offen.
- Keine Hauptschleife/Runden-State-Machine sicher isoliert.

## Werkzeuge und Build-Status

- Ghidra 12.1.4 Headless: `/home/user/tools/ghidra/ghidra_12.1.4_PUBLIC/support/analyzeHeadless`.
- C-Compiler `cc`/GCC und Python 3 sind vorhanden.
- Python-Skript `tools/rebuild_maincpu.py` baut nur `working/maincpu.bin`; in Phase 9 nicht erneut ausfuehren, ausser explizit beauftragt, da es die Arbeitsausgabe neu schreibt.
- CMakePresets `debug`/`release`, fuenf CTest-Ziele mit 49 Testfaellen. Debug, Release und ASan/UBSan/LeakSanitizer bestanden.
- Kein SDL3-Frontend und kein distributables Spiel-Executable. Alle automatisierten Tests sind headless; kein Original-68000-Code wurde emuliert.

## Naechster Schritt

Phase 20 ist der dokumentierte Abschlussstand. Weitere Arbeiten benoetigen einen neuen ausdruecklichen Auftrag und eine neue Freigabe.
