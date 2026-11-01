# Final Reconstruction Report

## Basis

Ziel ist MAME `sf2eb`, Street Fighter II: The World Warrior, World 910214. Vorhanden sind 23 ROM-Dateien: 8 Programm-, 12 Grafik- und 3 Sound-ROMs. Alle 23 SHA-256-/CRC-Pruefungen gegen die gespeicherten Referenzdaten stimmen. Acht in MAME gelistete PLDs fehlen lokal.

Das M68000-Programmabbild `working/maincpu.bin` besteht aus den acht belegten Even-/Odd-ROM-Lanes, 1.048.576 Bytes, SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`, CRC32 `94d86e85`. Ghidra 12.1.4 importierte es als Raw Binary bei Base `0x0`, Language `68000:BE:32:default`, Big Endian.

## Analyseumfang

Der gespeicherte Ghidra-Stand hat 353 Funktionsstarts und 11.356 ReferenceManager-Xrefs. Vollstaendige Katalog-/Mappingdaten: `reports/COMPLETE_FUNCTION_CATALOG.md`, `reports/function_coverage.csv` und `working/final_reference_inventory.log`.

| Einordnung | Ghidra-Starts | C-Status |
|---|---:|---|
| Vollstaendig und belastbar rekonstruiert | 0 | Kein voller originaler Funktionskoerper liegt vor. |
| CONFIRMED, partiell | 1 | `FUN_0000040e` bei `0x40e`: nur Coin-Control-/GFX-Clear-Side-Effects. |
| PROBABLE | 2 | `FUN_0007e486` und `FUN_0007e492`: gemeinsames Host-Overlap-Modell ohne rekonstruierte 68000-ABI. |
| UNKNOWN ohne adressgebundene C-Implementierung | 350 | Davon sind 62 in thematischen Berichten erwaehnt und 288 ohne solchen Berichtstreffer; Erwaehnung ist kein Verhaltensnachweis. |

Die drei C-Dateien und zwei Header liegen unter `src/`. Sie enthalten zwei adressgebundene Hostmodelle, sechs lokale Arithmetikhelper und eine Hilfs-Vektortabelle. Es gibt keine gesicherten Player-, Character-, Move-, Animation-, Hitbox-/Hurtbox- oder Game-State-Runtime-Strukturen.

## Reset-Zielkorrektur

Der Ghidra-Xref der LEA bei `0x41e` zeigt auf `0x426`. `JMP (A4)` bei `0x750` ist anhand des Registerdatenflusses statisch auf den dekodierten zweiten LEA-Einstieg `0x426` rueckfuehrbar; `0x42a` verzweigt nach `0x7cc`. `0x428` ist das Extension-Word der LEA bei `0x426`, kein Instruktionsstart. Der Ghidra-Decompiler bildet den indirekten Transfer nicht ab; kein dynamischer Reset-Trace wurde ausgefuehrt.

## Nicht rekonstruiert

Keine sichere ROM-Inputroutine, Main-Loop, vollstaendige Reset-/State-Machine, HP-/Schadensformel, Guard-/Knockbacklogik, Fighter-spezifische Move-Tabelle, Sprite-/Audioassetdekodierung oder vollstaendige Collision-Semantik. Die 3 Sound-ROMs sind identifiziert, nicht als PCM/ADPCM-Inhalte dekodiert.
