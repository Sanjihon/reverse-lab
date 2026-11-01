# Phase 10: Source-Validierung

Stand: 2026-11-01. Diese Pruefung verwendet das gespeicherte Ghidra-Projekt `working/SF2MainCPU.gpr` (Ghidra 12.1.4, `68000:BE:32:default`, Image Base `0x0`) und `working/maincpu.bin`. Autoanalyse wurde deaktiviert.

## Funktionsmetadaten und Zuordnungen

| Ghidra-Start | Ende inkl. | Bytes | Ghidra-Signatur (Decompiler) | C-Zuordnung | Bewertung |
|---|---|---:|---|---|---|
| `0x0000040e` | `0x00000751` | 46 | `void FUN_0000040e(void)` | `void recon_FUN_0000040e_prefix(void)` | Signatur ist kompatibel; Implementierung bildet nur belegte Side Effects ab und endet vor dem indirekten Sprung. Ghidra-Body-Ranges: `0x40e-0x425`, `0x73c-0x751`. |
| `0x0007e486` | `0x0007e491` | 12 | `void FUN_0007e486(void)` | `recon_FUN_0007e486_overlap_model(...)` | Wahrscheinliches Host-Modell; C-Parameter-/Rueckgabesignatur ist nicht als originale 68000-ABI belegt. |
| `0x0007e492` | `0x0007e52d` | 156 | `void FUN_0007e492(void)` | derselbe Collision-Wrapper | Getrennter Ghidra-Start, gemeinsamer Vergleichspfad; Host-Wrapper-Rueckgabe `bool` ist abgeleitet, nicht Ghidra-signiert. |

Start, Ende, belegte Bytes, Symbolquelle, direkte Call-Kanten, Datenreferenzen und die zugeordneten eingehenden/ausgehenden Xrefs fuer alle 353 Funktionen stehen in `reports/function_coverage.csv`. Bei Funktionen mit nicht zusammenhaengenden Ranges sind die Ghidra-Bytezahl und die inklusive Maximaladresse getrennt aufgefuehrt.

### Call- und Datenreferenzen der C-Zuordnungen

- `FUN_0000040e`: eingehender Kontrollfluss-Xref von `0x406` (`UNCONDITIONAL_JUMP`); zusaetzliche DATA-Xrefs zeigen von `0x7e62e` und `0x7e67e` auf die Adresse `0x40e`. Ausgehend schreibt die Funktion an `0x800030`, hat den PC-relativen DATA-Xref `0x41e -> 0x426`, springt bei `0x422` nach `0x73c` und schreibt im Loop an `0x900000`/`0x900002`. Der Loop verwendet A0/A1; `JMP (A4)` steht bei `0x750`.
- `FUN_0007e486`: direkte Caller sind vier Call-Sites in `FUN_0007e25e` (`0x7e270`, `0x7e292`, `0x7e2b4`, `0x7e2e6`). Im 12-Byte-Fragment verzweigt `0x7e48c` bedingt zu `0x7e492` und `0x7e490` zu `0x7e498`.
- `FUN_0007e492`: eingehende direkte Calls bei `0x7e570`, `0x7e8b0`, `0x7e8c8` sowie der bedingte Sprung von `0x7e48c`. Ghidra weist keine absoluten DATA-Xrefs aus; die Assembly verwendet Register-indirekte Objekt-/Descriptor-Zugriffe und A5-relative Scratch-Offsets.

Die Kollisionsinstruktionen zeigen Bytes an A3/A4-Descriptoroffsets `+0..+5`, Objektzugriffe ueber A2/A6 bei `+6`, `+10`, `+0x12` und Wortarithmetik fuer Extents und Deltas. Das stuetzt einen generischen 2D-Overlap-Vergleich. Es belegt keine konkrete Attack-/Hurtbox-Bedeutung oder die originalen C-Feldtypen.

Im C-Hostmodell ruft `recon_FUN_0007e486_overlap_model` die sechs Helper `add16`, `sub16`, `s16_from_bits`, `sign_extend_byte`, `load_s16_be` und `negate_s16_wrap` auf. `load_s16_be` und `negate_s16_wrap` rufen ihrerseits `s16_from_bits` auf. Diese source-only Beziehungen sind in den `c_helper_callers`-/`c_helper_callees`-Spalten der Coverage-CSV getrennt von den Ghidra-ROM-Kanten erfasst.

## Reset, Input und Kandidaten

`0x41e` decodiert als `LEA (0x6,PC),A4` mit Ghidra-DATA-Xref auf `0x426`. `0x422` verzweigt nach `0x73c`; dort aendert der Clear-Loop A0/A1, nicht A4. Bei `0x750` steht `JMP (A4)`. `0x426` ist als LEA dekodiert, `0x428` ist an dieser Adresse `NOT_DECODED`, und `0x42a` verzweigt nach `0x7cc`. Der Phase-9-Decompilerlauf fuer `0x40e` war erfolgreich, hat den indirekten Folgepfad aber nicht rekonstruiert.

Der frische Phase-10-Referenzlauf fand **0** Xrefs in MAMEs `IN1`-Fenster `0x800000-0x800007` und DIP-Fenster `0x800018-0x80001f`. Das schliesst dynamisch berechnete Zugriffe nicht aus; eine tatsaechliche Inputroutine bleibt UNKNOWN.

Der Decompiler meldete fuer sieben gezielt gepruefte Funktionen Erfolg: `0x40e`, `0x7d6e2`, `0x7d9b4`, `0x7f1b4`, `0x7e25e`, `0x7e486`, `0x7e492`. Erfolgreiche Dekompilierung ist kein Semantikbeweis. `0x7d9b4` (1.098 belegte Bytes) und `0x7f1b4` (1.244 Bytes) bleiben grosse Objekt-/Descriptor-Kandidaten; ihre Grenzen und die Zuordnung zu Character-/Move-Daten sind nicht belastbar. Keine Funktion wurde deshalb nachtraeglich umbenannt oder als Character-/Move-Implementierung klassifiziert.

## Datenstrukturen und C-Typen

- Integer-Aliases in `src/common/types.h`: `u8/s8` 1 Byte, `u16/s16` 2 Byte, `u32/s32` 4 Byte; `_Static_assert`s bestehen.
- `sf2_verified_vector` ist ein Hilfsrecord fuer die dekodierten Vektorwerte, kein nachgewiesenes Runtime-Struct. `memory_map.h` enthaelt MAME-verifizierte Adresskonstanten, keine hostseitige RAM-Abbildung.
- Keine verifizierten Player-, Character-, Move-, Animation-, Hitbox-/Hurtbox- oder Game-State-Strukturen. A5-relative Offsets und Objektfelder bleiben rohe Zugriffsbeobachtungen bzw. Kandidaten; unbekannte Feldsemantik wird nicht als C-Typ behauptet.
- Keine C-globalen Spielvariablen wurden nachgewiesen. Die Vektortabelle ist eine konstante Analysehilfe.

## Validierung und Integritaet

- Ghidra-Export: `InventorySf2Project.java` mit `-noanalysis`; 353 Funktionen, 11.356 ReferenceManager-Xrefs. Der gesonderte Phase-10-Exporter dekompilierte die sieben genannten Starts und las nur bereits dekodierte Instruktionen.
- Ghidra-Projektdateien vor/nach den `-noanalysis`-Laeufen bytegleich: `project.prp` SHA-256 `f55a557993cc461668783004aa3875e5c92a38f2bc7138c124355e14ccc64ed1`; Programmdatenbank SHA-256 `c24f693aa1e190bd415b4d08312aa553bd6a6b5bb90d7cec8c265d29386bb72c`.
- C-Syntaxcheck erfolgreich (Exit 0): `cc -std=c11 -Wall -Wextra -Werror -Isrc -fsyntax-only src/system/startup.c src/system/interrupts.c src/game/collision.c`.
- VS-Code-Diagnosen: keine Fehler in `src/` und den geaenderten Ghidra-Skripten.
- Helper-Verhaltenstest erfolgreich (Exit 0): `cc -std=c11 -Wall -Wextra -Werror -Isrc tests/collision_helpers_test.c -o /tmp/sf2_collision_helpers_test && /tmp/sf2_collision_helpers_test`. 15 Assertions decken `add16`, `sub16`, `s16_from_bits`, `sign_extend_byte`, `load_s16_be` und `negate_s16_wrap` ab. Dies testet C-Helper-Verhalten, nicht die ROM.
- Erfolgreich verhaltensgetestete Ghidra-Funktionen: **0**. `recon_FUN_0000040e_prefix` wurde wegen direkter MMIO-Zugriffe und `recon_FUN_0007e486_overlap_model` wegen `PROBABLE`-Semantik nicht ausgefuehrt.
- Original-ROMs: 23/23 SHA-256-Werte stimmen mit `reports/rom_manifest.csv` ueberein; Abweichungen 0. `working/maincpu.bin`: 1.048.576 Bytes, SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`, Sollwert stimmt. Original-ROMs und Image wurden nur lesend geprueft.
