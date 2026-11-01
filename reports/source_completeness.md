# Phase 10: Source-Rekonstruktion auf Vollstaendigkeit pruefen

Stand: 2026-11-01. Phase 10 und Paket 10 wurden als ein gemeinsamer Pruefauftrag bearbeitet. Ueberlappende Anforderungen wurden nicht doppelt ausgefuehrt. Es wurde keine Spielimplementierung begonnen.

## Bestand und erwartete Pfade

- Die angeforderten Pfade `reconstruction/src/` und `reconstruction/include/` existieren nicht. Die vorhandene Rekonstruktion liegt direkt unter `src/`.
- Vorhanden sind drei C-Dateien (`src/system/startup.c`, `src/system/interrupts.c`, `src/game/collision.c`) und zwei Header (`src/common/types.h`, `src/memory/memory_map.h`).
- `working/maincpu.bin` ist vorhanden (1.048.576 Bytes). Das vorhandene Ghidra-Projekt liegt unter `working/SF2MainCPU.gpr` und `working/SF2MainCPU.rep/`.
- Der bisherige Katalog, das Phase-9-Mapping und die Referenzinventare sind vorhanden. Frische Phase-10-Exporte liegen in `working/phase10_inventory.log` und `working/phase10_source_evidence.log`.
- Es wurden keine Testdateien, Testverzeichnisse oder Build-Konfigurationen (Make/CMake/Meson) gefunden. Ein vollstaendiges Buildsystem existiert nicht.
- Erwartete Implementierungen wie Input-Polling, Player-/Character-State, Move-/Animationstabellen, Game Loop und Damage-System fehlen. Sie sind nicht als abgeschlossene Arbeiten oder als C-Platzhalter zu zaehlen.

## Ghidra-Coverage

Der frische Read-only-Export aus dem vorhandenen Ghidra-Projekt enthaelt 353 eindeutige Funktionsstarts. Die CSV hat zusaetzlich sechs C-only-Helper-Zeilen, die ausdruecklich keine Ghidra-Adressen erhalten.

| Coverage-Klasse | Ghidra-Funktionen | Bedeutung |
|---|---:|---|
| Vollstaendig rekonstruiert | 0 | Kein C-Modell deckt eine Ghidra-Funktion vollstaendig und belastbar ab. |
| Partiell rekonstruiert, bestaetigt | 1 | `FUN_0000040e` bei `0x0000040e`; nur belegte Reset-Seiteneffekte. |
| Wahrscheinliches Host-Modell | 2 | `FUN_0007e486` und `FUN_0007e492`; ein C-Wrapper fuer zwei getrennte Ghidra-Starts. |
| Nur dokumentiert | 62 | In einem der 13 thematischen Phase-1-bis-9-Berichte per Adresse oder Ghidra-Name erwaehnt, aber ohne C-Implementierung. Das ist kein Verhaltensbeweis. |
| UNKNOWN | 288 | Weder C-Implementierung noch Treffer in den ausgewaehlten thematischen Berichten. |
| Platzhalter | 0 | Keine Stub-/TODO-Funktionskoerper in `src/` gefunden. |

Summe: 353 Ghidra-Funktionen. `Nur dokumentiert` und `UNKNOWN` bilden zusammen die 350 Funktionen ohne C-Implementierung. Die Quellen der Dokumentations-Treffer stehen je Zeile in `reports/function_coverage.csv`.

Der C-Bestand hat acht Funktionsdefinitionen: zwei adressgebundene C-Modelle und sechs arithmetische Helper in `collision.c`. Das Collision-Modell `recon_FUN_0007e486_overlap_model` ist `PROBABLE` und deckt zwei Ghidra-Starts ab; es ist keine wiederhergestellte 68000-ABI. Die sechs Helper haben keine 1:1-Ghidra-Funktion.

## Offene Untersuchungen

Fuenf getrennte Untersuchungsbereiche bleiben offen; Charakterdaten und Bewegungsdaten sind getrennt gezaehlt, weil Phase 10 sie separat auffuehrt:

1. Vollstaendiger Reset-/Initialisierungspfad nach dem indirekten Sprung.
2. Tatsaechliche ROM-Inputroutine; ein dynamisch berechneter I/O-Zugriff ist durch fehlende absolute Xrefs nicht ausgeschlossen.
3. Character-Daten und ihre Zuordnung zu Figuren.
4. Move-/Animationsdaten und ihre Semantik.
5. Genaue Collision-Semantik, insbesondere Attack-/Hurtbox-Klassen und ihre Verbindung zu Spielzustaenden.

Strukturfelder fuer Spieler, globale Laufzeitvariablen, Charaktere, Moves, Animationen, Kollisionen, Hitboxen/Hurtboxen und Spielzustaende bleiben separat UNKNOWN, soweit sie nicht ausdruecklich in den vorhandenen Berichten als rohe Speicherzugriffe oder Kandidaten ausgewiesen sind.

## Datenstrukturen und Tests

`types.h` verifiziert 8-/16-/32-Bit-Integerbreiten. `interrupts.c` enthaelt den Hilfs-Record `sf2_verified_vector` fuer Index und Zieladresse; er ist kein Byte-fuer-Byte-Layout der Vektortabelle. Damit gibt es einen Hilfs-Recordtyp, aber **null belastbar rekonstruierte Runtime-/Spieler-/Character-/Move-Strukturen**. Die C-Datei `collision.c` verwendet Bytepointer und explizite Offsets statt unbelegter ABI-Structs.

Vor Phase 10 gab es keine Testsuite. `tests/collision_helpers_test.c` prueft sechs isolierbare, deterministische C-Helper mit 15 Assertions erfolgreich. Diese Tests validieren nur die Host-Arithmetik, nicht Ghidra-ROM-Funktionen. Erfolgreich verhaltensgetestete Ghidra-Funktionen: **0**. Der Reset-Prefix greift direkt auf MMIO-Adressen zu; das Collision-Modell ist nur `PROBABLE`, daher wurden beide nicht getestet. Der C-Syntaxcheck ist ebenfalls kein Verhaltenstest.

## Widerspruch aus Vorberichten

Mehrere Phase-1-bis-9-Berichte nennen `0x428` als berechnetes A4-/Reset-Ziel. Der frische Ghidra-Xref fuer `LEA (0x6,PC),A4` bei `0x41e` zeigt stattdessen auf `0x426`; `0x428` ist in diesem Listing nicht dekodiert und liegt innerhalb der Instruktion bei `0x426`. Die A4-Registerverwendung bleibt bis `JMP (A4)` bei `0x750` erhalten. Der wahrscheinlich ausgefuehrte Sprung geht somit nach `0x426`, von wo die dekodierte `BRA.W` bei `0x42a` nach `0x7cc` fuehrt. Der Ghidra-Decompiler bildet den indirekten Transfer nicht ab; der Sprungpfad ist daher als Registerdatenfluss-Inferenz, nicht als explizite Ghidra-Call-Kante dokumentiert. Die fruehere `0x428`-Aussage ist fuer diesen Pfad nicht zu uebernehmen.

Der bisherige C-Kommentar und das Phase-9-Mapping wurden auf den Xref-Befund aktualisiert. Aeltere narrative Phasenberichte mit `0x428` bleiben als historische Artefakte bestehen; diese Phase-10-Auswertung ist die aktuelle Korrektur.

## Abschluss

Phase 10 / Paket 10 ist abgeschlossen. Keine Phase nach Phase 10 wurde begonnen. Vor Phase 11 ist ausdrueckliche Freigabe erforderlich.
