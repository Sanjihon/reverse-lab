# Phase 12: Zuordnung Originalbefunde zu neuen Modulen

Diese Tabelle verhindert, dass neue Architekturannahmen als rekonstruierte Originalfunktion erscheinen. `implementation/` enthaelt in Phase 12 noch keine Spiellogik.

| Originalbefund / Rekonstruktion | Evidenzstatus | Geplantes Modul | Verwendungsgrenze |
|---|---|---|---|
| Reset `FUN_0000040e`, `recon_FUN_0000040e_prefix` | CONFIRMED nur fuer dokumentierte Coin-Control-/GFX-Clear-Side-Effects; Prefix ist unvollstaendig | `core` Lifecycle nur als Vergleich | Keine MMIO-Zugriffe, Startupsequenz oder originale Initialisierungsannahme in den neuen Engine-Start kopieren. |
| `FUN_0007e486` / `FUN_0007e492`, `recon_FUN_0007e486_overlap_model` | PROBABLE; Host-ABI abgeleitet | `combat` / `Collision System` | Kann eine Hypothese fuer getrennte Geometrieabfragen liefern. Keine behauptete Attack-/Hurtbox- oder Fighter-Semantik; neues Modul erhaelt eigene Regeln und Tests. |
| Sechs lokale C-Arithmetikhelper aus `src/game/collision.c` | Host-Arithmetik getestet, keine ROM-Funktion | kein automatisches Mapping; ggf. spaeter reviewed als Utility | Nicht kopieren, bevor Namensraum, Vorzeichen-, Wraparound- und Alignmentvertrag als neue API festgelegt ist. |
| `sf2_verified_vectors` in `src/system/interrupts.c` | Beobachtete Vektorwerte, kein Runtime-Struct oder Handler | kein Spielmodul | Nur Analysemetadatum, nicht fuer Spielzustaende verwenden. |
| `src/memory/memory_map.h` | MAME-verifizierte Hardwareadressen/Portmasken fuer das CPS-Board | kein Plattformmemory im neuen Spiel | Keine Hostpointer-/SDL-Abbildung. Neue Implementierung nutzt logische Inputaktionen statt CPS-MMIO. |
| `FUN_0007d6e2`, `FUN_0007e25e` und Dispatch-Ziele | Collision-/Objektzustands-Callpfad; Fachsemantik unvollstaendig | `combat`, optional `player` | Nur als Recherchehinweis; keine Hitreaktion, Prioritaet oder Zustandsnamen als Originalfakt uebernehmen. |
| `FUN_0007d9b4`, `FUN_00083588`, `FUN_00083838`, `FUN_000839ea` | UNKNOWN fuer Player-/Game-State-Bedeutung | `game`, `player`, `core` | Keine wiederverwendbaren State-Machine-Anforderungen belegt. |
| `FUN_0007f1b4`, `FUN_00009314`, GFX-Descriptorzugriffe | Rendering-/Objektlisten-Kandidaten, teils PROBABLE | `graphics`, `animation` | Keine Spriteformate, Pixelassets oder Rendererimplementierung als verifiziert behandeln. |
| MAME IN1/DIP-Belegung | Hardware-Portbeschreibung; keine Leseroutine durch Ghidra-Xrefs gefunden | `input` | Logische P1/P2-Aktionen sind ein neuer Inputvertrag; Active-low Bitmuster nur als Hardwarereferenz. |
| Character-/Move-/Animation-/Audio-/UI-Inhalte | Keine belastbaren Structures/Datensaetze oder genehmigte Medien vorgefunden | `character`, `animation`, `audio`, `graphics` | Erst neue eigene oder nachweislich lizenzierte Inhalte; Original-ROMs und daraus extrahierte Assets bleiben ausgeschlossen. |

## Trennregeln

- Quellcode aus `src/` bleibt die dokumentierte, partielle Reverse-Engineering-Rekonstruktion. Er wird weder verschoben noch zum Buildinput der neuen Anwendung.
- Die spaetere Implementierung muss neue Module als **neu entwickelt** markieren, bis fuer eine konkrete Regel belastbarer Originalcodevergleich vorliegt.
- ROM-Dateien, `working/maincpu.bin` und Ghidra-Datenbank sind Analysebelege und keine Runtime-Abhaengigkeiten.
