# Offene Fragen und naechste Untersuchungen

## Kritisch fuer korrekte Funktionssemantik

1. **Reset-/Mainpfad:** Phase 10 zeigte statisch, dass die LEA bei `0x41e` auf `0x426` verweist; `JMP (A4)` bei `0x750` fuehrt zum dekodierten zweiten LEA-Einstieg `0x426`, und `0x42a` verzweigt nach `0x7cc`. Offen bleiben dynamische Erreichbarkeit, vollstaendige Initialisierungswirkung und die tatsaechliche Main-Loop.
2. **Indirekte Kontrollfluesse:** ReferenceManager meldet 513 `COMPUTED_JUMP`- und 143 `UNCONDITIONAL_JUMP`-Kanten; direkte Funktionskoerperstatistik meldet 0 computed calls. Zielsatz fuer `JMP (A4)` bei `0x750`, `0x766`, `0x7ae` und weitere berechnete Spruenge ist nicht vollstaendig aufgeloest.
3. **Ueberlappende Funktionen:** Decompiler-Warnungen und auffaellige Koerper wie `FUN_00002540` (`0x2540-0xa4c1`, 2.378 belegte Bytes) koennen Call-/Switch-Interpretation verfalschen. Bad instruction data/Ueberlappungen an `0x3812/0x3814`, `0x7dac6/0x7dac8` und im Bereich `0x838xx-0x89xxx` gezielt pruefen.
4. **Autoanalyse-Warnungen:** Ghidras Standard-Autoanalyse meldete 488 nicht-fatale PCode-Decompiler-Warnungen. Ghidra-Language `68000:BE:32:default` dekodiert den 68040-Superset; alternative Instruktions-Entries oder Datenbereiche koennen false functions erzeugen.

## Spielerinput und Spielablauf

- Einen direkten oder indirekten ROM-Zugriffspfad zu MAME `IN1` (`0x800000-0x800007`) und DIP (`0x800018-0x80001f`) finden; bisher keine absoluten Ghidra-Code-Xrefs.
- Port `IN2`-/Kick-Button-Zugriff ueber CPS-B/Input-selector klaeren; MAME deklariert die Bits, aber keine direkte 68000-Adresse wurde hier belegt.
- Ringpuffer `FUN_000021e2` (`A5+0xa2`, Index `A5+0x1e`) ueber alle Caller bis zur Quelle der Ereignisworte verfolgen; Inputbuffer-Hypothese bleibt unbewiesen.
- `FUN_00083588`, `FUN_00083838`, `FUN_000839ea` als moegliche periodische Dispatch-/Updatepfade aufrufen/return und Taskkadenz untersuchen; keine Hauptschleife ist sicher.
- UI-Texte an `0x8dbbd-0x90fc9` haben fuer 14 Ghidra-Stringobjekte null Xrefs. Pointertabellen, indirekte Stringreferenzen und State-/Screen-IDs gesondert untersuchen.

## Kampf- und Physiklogik

- Collider-Descriptor `*(A2+0x1a)`, Datenblock `*(A2+0x34)`, `A3/A4` Offsets und A5-Scratch `-0x7538..-0x7511` typisieren; Angriff und Verteidigung unterscheiden.
- `FUN_0007e25e`/`FUN_0007e486`-Achsenauswahl gegen Datenstrukturen und bekannte Objektklassen korrelieren. Der gepruefte Pfad ist generische X/Y-Ueberlappung, nicht bewiesene Hitbox-vs-Hurtbox.
- Von Kollisionshandlern zu Lebenspunkten, Damage Tables, Hitstun, Guardstun, Trefferprioritaet und Knockback weiterverfolgen. Bisher keine belegte HP-/Schadensformel.
- Blockrichtung, Invulnerabilitaet, Sprung-/Lufttreffer, Spezialbewegungen, Angriff-Startup/Active/Recovery und Character-spezifische Tabellen identifizieren.
- `FUN_0007d6e2`-Statuswert `0x78`, `A2+0x2a` und weitere A2/A5-relative Felder typisieren, bevor sie als Hitstun/HP interpretiert werden.

## Vollstaendigkeit und Artefakte

- Alle 353 Ghidra-Funktionen sind im `reports/COMPLETE_FUNCTION_CATALOG.md` aufgefuehrt. Nicht semantisch beschriebene Namen bleiben `FUN_...` bzw. Ghidra-`caseD_...`; keine manuellen Umbenennungen.
- Alle 11.356 vom aktuellen Ghidra-ReferenceManager gemeldeten Kanten stehen in `working/final_reference_inventory.log`. Die Zaehlung ist vollstaendig fuer den aktuellen gespeicherten Projektzustand, aber sie beweist keine Entdeckungs-Vollstaendigkeit fuer unidentifizierte Code-/Datenbereiche.
- Die frueheren Phase-5/6-Funktionszaehler sind historische Snapshots. Phase 7 legte acht Tabellenhandler an; final sind es 353 Funktionen.
- Keine Laufzeitemulation, Hardwarebeobachtung, dynamische Eingabesequenz oder echte Platinenmessung wurde ausgefuehrt. Die 68000-Programm-ROMs wurden weder ausgefuehrt noch als Ghidra-ROM-/Board-Emulation getestet.
