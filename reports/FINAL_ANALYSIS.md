# Street Fighter II: Abschlussanalyse der Ghidra-Phasen

Stand: 2026-10-31. Ziel ist das vorhandene MAME-Set `sf2eb`, Street Fighter II: The World Warrior (World 910214). Die Original-ROMs in `sf2/` wurden ausschliesslich gelesen und nicht veraendert.

## Kurzfazit

- Phase 1 identifizierte 23 vorhandene ROMs; alle 23 CRC32-Werte stimmen mit MAME `sf2eb` ueberein. Acht in MAME gelistete PLDs fehlen im lokalen Verzeichnis.
- Phase 2 belegte die tatsaechlichen `ROM_LOAD16_BYTE`-Offsets der acht 68000-ROMs: vier Even/Odd-Paare bei `0x00000/1`, `0x40000/1`, `0x80000/1`, `0xc0000/1`.
- Phase 3 erzeugte `working/maincpu.bin` mit 1 MiB, SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`, CRC32 `94d86e85`; alle Bytes wurden laneweise gegen die Quelldateien verglichen.
- Phase 4 importierte das Image in Ghidra 12.1.4 als Raw Binary, Language `68000:BE:32:default`, Big Endian, Base `0x00000000`. MAMEs `main_map` verifiziert ROM bei `0x000000-0x3fffff`; der Dump enthaelt die ersten `0x100000` Bytes.
- Der aktuelle gespeicherte Ghidra-Stand hat 353 eindeutige Funktionsstarts und 11.356 ReferenceManager-Cross-References. Der vollstaendige Funktionskatalog ist [COMPLETE_FUNCTION_CATALOG.md](COMPLETE_FUNCTION_CATALOG.md); jede Xref-Kante steht in `working/final_reference_inventory.log`.
- Sicher identifiziert sind Reset-/Exception-Stubs und ein generischer geometrischer X/Y-Overlap-Pfad. Eine vollstaendige Fighter-Gameplay-Analyse, Input-Leseroutine oder Schadensformel wurde nicht nachgewiesen.

## Programm- und Adressgrundlage

`sf2eb` wird in MAME mit `cps1_10MHz` registriert; dieses Profil bindet `main_map`. Die MAME-Definition mappt ROM bei `0x000000-0x3fffff`, GFX-RAM bei `0x900000-0x92ffff` und Main-RAM bei `0xff0000-0xffffff`. Der Ghidra-Import bildet nur `working/maincpu.bin` als `ram`-Block `0x000000-0x100000` (Endadresse exklusiv) ab.

Resetvektor 1 bei `0x04` enthaelt PC `0x0000040e`; Vektor 0 enthaelt SSP `0x00000000`. Die ersten Instruktionen bei `0x40e` schreiben an CPS Coin-Control `0x800030`; die LEA bei `0x41e` referenziert `0x426` und verzweigt nach `0x73c`. Dort wird `0x900000-0x903fff` geloescht. Der anschliessende `JMP (A4)` bei `0x750` ist statisch auf den dekodierten LEA-Einstieg `0x426` rueckfuehrbar; `0x428` ist deren Extension-Word, und `0x42a` verzweigt nach `0x7cc`. Die vollstaendige Reset-/Main-Loop-Erreichbarkeit wurde nicht dynamisch validiert.

## Ghidra-Funktionen und Cross-References

Der finale Audit lief gegen das aktuelle persistente Projekt mit `-noanalysis`; er hat keine globale Autoanalyse erneut gestartet. Ghidra-Namen wurden nicht manuell umbenannt. 349 Symbole haben Quelle `DEFAULT`; vier Switch-Case-Labels haben Quelle `ANALYSIS` (`caseD_63`, `caseD_0`, `caseD_a7`, `caseD_10`). Acht weitere Default-Funktionen bei `0x7e306-0x7e3a6` wurden aus den Offsets der Kollisions-Dispatch-Tabelle angelegt.

| Ghidra ReferenceManager-Typ | Anzahl |
|---|---:|
| `COMPUTED_JUMP` | 513 |
| `CONDITIONAL_JUMP` | 1.009 |
| `DATA` | 8.677 |
| `READ` | 94 |
| `READ_WRITE` | 6 |
| `UNCONDITIONAL_CALL` | 788 |
| `UNCONDITIONAL_JUMP` | 143 |
| `WRITE` | 126 |
| **Gesamt** | **11.356** |

Das Funktionsinventar zaehlt separat 636 direkte Call-Edges und 975 funktionsgebundene Datenzielgruppen. Diese Werte haben eine andere Bezugsmenge als die 11.356 ReferenceManager-Kanten und sind nicht additiv. Das Inventarskript fand keine computed-call-Referenz in Funktionskoerpern; der ReferenceManager meldet dennoch 513 computed jumps. Das zeigt, warum indirekte Sprungziele nicht als vollstaendig aufgeloest gelten.

## Untersuchte Systeme

- **ROM/Memory:** Set, Chip-Paare, Lanes, MAME-Regionen und 68000-Basis sind quellbelegt; siehe `reports/mame_rom_definition.md`, `reports/memory_layout.md` und `reports/reconstruction_report.md`.
- **Start/Exceptions:** Vektorbereich `0x000-0x0ff`; Reset `0x40e`; Bus-/Address-Error-Recovery `0x400`; gemeinsames `RTE` bei `0x40c`; TRAP-Handler im Bereich `0xb24-0xd34`.
- **Spieler-Eingabe:** MAME belegt active-low P1/P2-Richtungs- und Buttonmasken. Im ROM wurde keine sichere Funktion gefunden, die `IN1`/DIP-Portadressen liest.
- **Objekt-/Bewegungszustand:** `FUN_0007d6e2`, `FUN_0007d9b4` und `FUN_0007f1b4` zeigen status-, koordinaten- oder descriptorbezogene Operationen; ihre Zuordnung zu Kaempferbewegung ist nicht sicher.
- **Kollision:** `FUN_0007d6e2` ruft `FUN_0007e25e`; `FUN_0007e486` berechnet Facing-korrigierte X/Y-Positionen und vergleicht Extents. Das ist stark mit generischem 2D-Box-Overlap vereinbar; Attack-/Hurtbox-Identitaet bleibt offen.
- **Grafik/Scene:** `FUN_00009314`, `FUN_00009476` und `FUN_0007f1b4` zeigen GFX-/Layer-/Sprite-Descriptorverarbeitung, keine bewiesene Trefferlogik.
- **UI/Game-States:** lesbare Strings existieren, aber 14 passende Ghidra-Stringobjekte hatten null eingehende Referenzen. Titel, Auswahl, Kampf, Sieg/Niederlage und Rundenwechsel sind keiner Routine sicher zugeordnet.
- **Schaden/Block/Knockback:** keine HP-Formel, Blockpruefung oder sicherer Knockbackpfad gefunden.

## Qualitaetssicherung

**Direkt in Ghidra verifiziert:** Language/Base, Imageblock, Reset-/Exception-Instruktionen, die 353 Funktionsstarts samt Ghidra-Namen, alle vom ReferenceManager gemeldeten 11.356 Kanten, Kollisions-Call-Edges, 13 Kollisions-/Axis-/Dispatch-Decompilergebnisse und die neu angelegte komplette Funktionsliste.

**Direkt in MAME verifiziert:** Setname `sf2eb`, LOAD-Offsets/ROM-Groessen, `main_map`, CPS-Eingabemasken und genannte Hardwarebereiche.

**Statische Interpretation:** Ringpuffer bei `FUN_000021e2`; Objekt-/Animationsstatus bei `FUN_0007d6e2`/`FUN_0007d9b4`; Sprite-/Objektlisten-Emission bei `FUN_0007f1b4`; generische Collider-/Boxsemantik von `FUN_0007e25e`/`FUN_0007e486`.

**Offen:** Hitbox gegen Hurtbox, Attack-IDs, Schaden/HP, Blocken, Knockback, konkrete Fighter-States, Inputpolling, Main Loop, String-Xrefs und indirekter Reset-Folgepfad.

Ghidras Standard-Autoanalyse meldete 488 nicht-fatale PCode-Warnungen. Es gab keine Laufzeitemulation. Die Phase-5-Berichte nennen 345 Funktionen, weil sie vor Anlage der acht Kollisions-Tabellenhandler erfasst wurden; der aktuelle Stand mit 353 Funktionen ist im Phase-8-Audit dokumentiert. Keine Original-ROMs wurden veraendert.
