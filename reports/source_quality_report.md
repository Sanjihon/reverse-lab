# Phase 9: Qualitaets- und Buildbericht

## Umfang und Zaehlweise

- Adressgebundene C-Rekonstruktionen: **2 C-Funktionen**.
- Status der C-Funktionen: **1 `CONFIRMED` partieller Reset-Seiteneffekt-Prefix**, **1 `PROBABLE` generisches Overlap-Modell**, **0 implementierte `UNKNOWN`-Funktionen**.
- Das Overlap-Modell mappt auf **zwei getrennte Ghidra-Function-Starts** (`0x7e486`, `0x7e492`); darum hat die CSV **1 CONFIRMED + 2 PROBABLE + 350 UNKNOWN = 353 Ghidra-Funktionszeilen**.
- C-Unterstuetzungsfunktionen ohne 1:1-Ghidra-Adresse: **6** (16-Bit-Arithmetik, signed wrap, Byte-Signextension, Big-Endian-Load).
- C-Funktionsdefinitionen insgesamt: **8** (2 adressgebundene Rekonstruktionen + 6 Helfer).
- Erstellte C-Dateien: **3** (`startup.c`, `interrupts.c`, `collision.c`); Header: **2** (`types.h`, `memory_map.h`); Source-Dateien insgesamt: **5**.
- Direkt dargestellte Datenstrukturen: **1** logischer Vektor-Recordtyp (`sf2_verified_vector`); **0** belastbare Player-/Character-/Move-/Hitbox-Structs.
- Ghidra-Funktionen ohne adressgebundene C-Implementierung: **350**, in der Mapping-CSV als `UNKNOWN`.

## Compilerpruefung

Ausgefuehrter strenger Syntaxcheck:

```sh
cc -std=c11 -Wall -Wextra -Werror -Isrc -fsyntax-only src/system/startup.c src/system/interrupts.c src/game/collision.c
```

Der Befehl endete ohne Diagnoseausgabe und Exit-Code 0. Das ist ein Syntax-/Warnungscheck, kein vollstaendiger Link oder Spielbuild. Ein aus stdin gebauter Typgroessen-Probe-Run meldete:

```text
u8=1 s8=1 u16=2 s16=2 u32=4 s32=4
```

Die `_Static_assert`s in `src/common/types.h` decken dieselben Breiten und `CHAR_BIT==8` ab.

## Ghidra-Quellenachweis

Das bestehende Projekt wurde mit `-noanalysis` ausgelesen. `tools/ExportSf2SourceEvidence.java` exportierte Instruktionsbytes, Ranges und Xrefs in `working/phase9_source_evidence.log`. Der Kollisions-Decompilation-Lauf ist in `working/phase9_decompiler.log` protokolliert. Das Ghidra-zu-C-Mapping wurde mit `tools/ExportSf2SourceMapping.java` aus 353 aktuellen Funktionsobjekten erzeugt.

Ghidra-Name, Start-/Endadressen, Byteumfang und Symbolquelle fuer alle 353 Funktionen stehen in `reports/source_function_mapping.csv`. Keine Ghidra-Funktion wurde manuell umbenannt. C-Funktionen besitzen explizite `recon_`-Namen mit Adresse/Scope.

## Integritaet und Einschraenkungen

- SHA-256 aller **23/23 vorhandenen Original-ROMs** stimmt weiterhin mit `reports/rom_manifest.csv` ueberein; Abweichungen: 0.
- `working/maincpu.bin` behielt SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`.
- Keine Original-ROM und kein rekonstruiertes Image wurde geschrieben.
- Es existiert keine `main()`/Buildsystemintegration und es wurde kein lauffaehiges Arcade-Programm erstellt oder ausgefuehrt.
- Die Resetimplementierung ist partiell: der indirekte Transfer wird im C-Prefix nicht modelliert. Phase 10 zeigte statisch, dass der Sprung bei `0x750` zu `0x426` fuehrt; `0x428` ist das Extension-Word der zweiten LEA. Der vollstaendige Reset-/Mainpfad ist nicht dynamisch validiert.
- Das Collision-C-Modell ist **PROBABLE** und kein wiederhergestellter Originalquelltext. Es nutzt explizite Pointer-/Outputparameter statt einer unbewiesenen Original-ABI/Struct-Deklaration.
- Der C-Compiler-Test verifiziert nur Hostsyntax und Integerbreiten. 68000-Flags, Hardwaremapping und Laufzeitverhalten sind nicht auf Hardware validiert.