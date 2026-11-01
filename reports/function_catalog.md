# Ghidra-Funktionskatalog

Dieser Bericht enthaelt den Phase-5-Snapshot des gespeicherten Projekts: 345 Funktionen, 636 funktionsgebundene direkte Call-Edges und 975 funktionsgebundene Datenreferenzen. In Phase 7 wurden acht Funktionen an aus der Kollisions-Dispatch-Tabelle berechneten Zielen hinzugefuegt. Der finale Phase-8-Audit zaehlt daher 353 Funktionen; die vollstaendige aktuelle Liste und alle aktuellen Cross-References stehen in `reports/COMPLETE_FUNCTION_CATALOG.md` und `working/final_reference_inventory.log`. Die 14 in Phase 5 angelegten Vektor-Handler und acht Tabellenhandler behielten ihre Ghidra-Standardnamen; keine Funktion wurde manuell umbenannt.

Funktionsgroessen sind die von Ghidra ermittelte Zahl belegter Adressbytes im Funktionskoerper. Bei disjunkten Koerpern kann die inklusive Endadresse eine groessere Adressspanne als die Groesse suggerieren. `FUN_...`-Namen sind unveraenderte Ghidra-Standardnamen. Die Registerangaben stammen aus Ghidras Instruction-Input/Result-Objekten; Subregisteransichten und Flags erscheinen teils separat.

## Sicher identifizierte Eintraege

### `0x00000400` - Exception-Recovery fuer Bus-/Address-Error

- Start/Ende/Groesse: `0x400-0x40b`, 12 Byte; Ghidra-Name: `FUN_00000400`.
- Register: liest `A5`, `SP`; schreibt `SP`.
- Aufrufe/Datenreferenzen: keine direkten Aufrufe, keine Ghidra-Datenreferenzen.
- Beleg/Funktion: Vektoren 2 und 3 zeigen auf `0x400`. Ghidra dekodiert `addq.l #8,SP`, danach bei `0x402` `lea (0,A5),SP`, dann bei `0x406` `jmp 0x40e`. Das setzt einen bekannten Stack und startet den Resetpfad neu.
- Konfidenz: **hoch** fuer den Stub und den Sprung; keine Aussage zur Ursache des ausloesenden Fehlers.

### `0x0000040c` - gemeinsamer Interrupt-Ruecksprung

- Start/Ende/Groesse: `0x40c-0x40d`, 2 Byte; Ghidra-Name: `FUN_0000040c`.
- Register/Aufrufe/Datenreferenzen: keine erfassten Register, keine direkten Aufrufe, keine Datenreferenzen.
- Beleg/Funktion: Opcode `4e73` ist `RTE`. Die Vektoren fuer spurious interrupt und Level 1 sowie Level 3-7 zeigen auf diese Adresse.
- Konfidenz: **hoch**.

### `0x0000040e` - Reset-PC / Reset-Einstieg

- Ghidra-Koerper: `0x40e-0x751`, 46 belegte Adressbytes; Name: `FUN_0000040e`.
- Register: liest `A0,A1,A4,CF,ZF`; schreibt `A0,A1,A4,CF,NF,VF,XF,ZF`.
- Aufrufe/Datenreferenzen: keine direkten Call-Edges; vier eindeutige Referenzziele: `0x800030`, `0x426`, `0x900000`, `0x900002` (`0x800030` kommt zweimal vor).
- Beleg/Funktion: Vektor 1 enthaelt `0x40e`. Listing: schreibt `0x80` und danach `0` nach CPS-Coin-Control `0x800030`, setzt `A4` PC-relativ und verzweigt zu `0x73c`. Dort wird GFX-RAM geloescht und ueber `jmp (A4)` weitergesprungen.
- Einschraenkung/Konfidenz: **hoch** fuer Vektor und dekodierte Instruktionen, **niedrig** fuer Ghidras Funktionsgrenze/Decompiler-Semantik: der dekompilierte Koerper stimmt auffaellig mit dem bei `0x400` ueberein. Der Ghidra-Xref der LEA bei `0x41e` zeigt auf `0x426`; `JMP (A4)` bei `0x750` ist statisch dorthin rueckfuehrbar. `0x428` ist das Extension-Word der zweiten LEA; der Decompiler bildet den indirekten Kontrollfluss nicht ab.

### `0x00000b24` - TRAP-#0-Recordhandler

- Start/Ende/Groesse: `0xb24-0xb41`, 30 Byte; Name: `FUN_00000b24`.
- Register: liest `A0,A1,A5,D0w,D1w,D2w,ZF`; schreibt `A1,CF,NF,VF,ZF`.
- Aufrufe/Datenreferenzen: keine direkten Aufrufe oder statischen Datenreferenzen.
- Beleg/Funktion: Vektor 32 (TRAP #0) zeigt auf `0xb24`. Der Code adressiert einen Eintrag relativ zu `A5-0x8000` plus `D0`, prueft ein Statusbyte und initialisiert bei leerem Eintrag Status `0x0c00`, A0 und D1/D2-Felder; Rueckkehr mit `RTE`.
- Konfidenz: **hoch** fuer Vektor und beobachtete Record-Operation; Bedeutung des Records unbekannt.

### `0x00000b8a` / Vektorziel `0x00000b8c` - TRAP-#1-Kontext

- Start/Ende/Groesse: `0xb8a-0xbad`, 36 Byte; Name: `FUN_00000b8a`. Der Vektor zeigt zwei Bytes nach dem Ghidra-Funktionsanfang auf `0xb8c`.
- Register: liest `A0,A5,D0,SP`; schreibt `A0,CF,D0,NF,SP,VF,ZF`.
- Aufrufe/Datenreferenzen: ein direkter Aufruf zu `FUN_00000a5e`; keine Datenreferenzen.
- Beleg/Funktion: Der Bereich enthaelt am Anfang einen `TRAP #1`-Aufruf; ab dem Vektorziel wird der Stack bereinigt, ein A5-relativer Kontext geladen, mehrere Felder werden geloescht und `0xa5e` aufgerufen.
- Konfidenz: **mittel** fuer die Service-/Kontextfunktion, **hoch** fuer das Vektorziel. Wegen des Eintritts mitten in den Ghidra-Funktionskoerper wird nicht umbenannt.

### `0x00000be4` - TRAP-#2-Recordfreigabe

- Start/Ende/Groesse: `0xbe4-0xc07`, 36 Byte; Name: `FUN_00000be4`.
- Register: liest `A4,A5,D0w,D1,ZF`; schreibt `A4,CF,D1,NF,VF,ZF`.
- Aufrufe/Datenreferenzen: keine direkten.
- Beleg/Funktion: Vektor 34 (TRAP #2) zeigt auf `0xbe4`; der Code prueft den Eintrag bei `A5-0x8000 + D0` und loescht bei gesetztem Status die Felder `+0x10..+0x1c` sowie das Statusbyte; danach `RTE`.
- Konfidenz: **hoch** fuer Record-Loeschung, **mittel** fuer die Bedeutung als Freigabe.

### `0x00000c12` - TRAP-#8-Slot-/Listenverwaltung

- Start/Ende/Groesse: `0xc12-0xc5b`, 74 Byte; Name: `FUN_00000c12`.
- Register: liest `A3,A4,A5,D0,D1b,D7w,ZF`; schreibt `A3,A4,CF,D0,D7w,NF,VF,XF,ZF`.
- Aufrufe/Datenreferenzen: keine direkten.
- Beleg/Funktion: Vektor 40 (TRAP #8) zeigt auf `0xc12`. Der Code scannt bis zu acht Datensaetze im Abstand `0x20` ab `A5-0x7f00`, sucht ein aktives Element mit passendem Byte bei `+0x16`, leert den Slot, legt den Zeiger auf eine A5-relative Liste bei `-0x7df8` und erhoeht einen Zaehler bei `-0x7df4`.
- Konfidenz: **mittel-hoch** fuer eine Slot-/Freiliste; konkrete Objektart unbekannt.

### `0x00000cb8` - TRAP-#3-Aufrufwrapper

- Start/Ende/Groesse: `0xcb8-0xcc3`, 12 Byte; Name: `FUN_00000cb8`.
- Register: Ghidra erfasst `A0-A6,D0-D7,SP` als gelesen und geschrieben; Stackreferenz `Stack[-0x3c]`.
- Aufrufe/Datenreferenzen: 28 direkte Caller, keine normalen Call-Edges; eine Stack-Datenreferenz.
- Beleg/Funktion: Listing: `MOVEM.L` sichert Register, `TRAP #3`, `MOVEM.L` stellt Register wieder her, `RTS`. Ghidras Decompiler bildet den Trap als `__m68k_trap(3)` ab.
- Konfidenz: **hoch** fuer Wrapper und Trap-Nummer. Der Name im Projekt bleibt unveraendert.

### `0x00000cc4` - TRAP-#3-Handler / gemeinsamer Dispatch

- Start/Ende/Groesse: `0xcc4-0xceb`, 40 Byte; Name: `FUN_00000cc4`.
- Register: liest `A0,A1,A5,D0b,D0w,D1w,SP,USP,ZF`; schreibt `A0,A1,CF,D0w,D1w,NF,SP,VF,ZF`.
- Aufrufe/Datenreferenzen: ein direkter Aufruf zu `FUN_00000a5e`; keine Datenreferenzen.
- Beleg/Funktion: Vektor 35 (TRAP #3) zeigt auf `0xcc4`. Der Handler baut ein Statuswort aus D0, speichert USP und Teile des Exception-Stacks in einen A5-relativen Kontext und verzweigt in den gemeinsamen Dispatcher `0xa5e`.
- Konfidenz: **mittel-hoch** fuer Dispatcher-Setup; Zweck von `0xa5e` bleibt offen.

## Vermutete Spiel-/Grafikfunktionen

| Start-Ende / Groesse | Ghidra-Name | Register, Calls, Datenreferenzen | Vermutung, Begruendung und Konfidenz |
|---|---|---|---|
| `0x21e2-0x2223`, 66 B | `FUN_000021e2` | liest `A4,A5,D0w,D1w,D2w,SP`; schreibt `A4,D2w,SP`/Flags; 17 Caller, keine Calls/Refs | Ringpuffer-Schreiber: prueft A5-relative Kontrollfelder, schreibt D0/D1 bei `A5+0xa2+Index` und wrappt Index mit `&0xfc`. **Mittel-hoch**. |
| `0x50ce-0x50db`, 14 B | `FUN_000050ce` | D5/D6/SP/Flags; 17 Caller, keine Calls/Refs | Ghidra dekompiliert nur `return`; sichere No-op-/Return-Stelle, fachlicher Zweck unbekannt. **Hoch** fuer beobachtetes Verhalten, **niedrig** fuer Rolle. |
| `0x9476-0x98b7`, 606 B | `FUN_00009476` | liest `A0,A1,A2,A5,D1w,D3w,D5,SP`; schreibt `A0,A1,A2,A6`; 2 Caller, 9 Calls, 16 Refs | Erzeugt wiederholt GFX-Kommandoworte (`0x81ff`, `0x81de`, `0x81ee`) und schreibt in von A0 gelieferte Strukturen; nutzt Tabellen bei `0x96be` und Helfer `0x935c`, `0x945a`, `0x9850`, `0x98b8`. Wahrscheinlicher Grafik-/Layer-Setup. **Mittel**. |
| `0x7f1b4-0x7f8a3`, 1.244 B | `FUN_0007f1b4` | liest `A0-A6,D0w-D7w,SP,Flags`; schreibt `A0,A2,A3,A6,D0w-D7w,SP,Flags`; 13 Caller, Calls `0x7f8a4`, `0x7f56a`, `0x7f62e` (vier Call-Sites), 4 Refs | Verarbeitet Objekt-/Tile-Daten, berechnet Koordinaten, clippt gegen `0x200` und emittiert Vier-Wort-Eintraege in einen Ausgabepuffer. Wahrscheinliche Sprite-/Objektlisten-Emission. **Mittel**. |
| `0x7d9b4-0x849f9`, 1.098 B | `FUN_0007d9b4` | liest/schreibt `A0-A6,D0-D7,SP,Flags`; 1 Caller, 23 Calls, 18 Refs | A5/A2/A3-relative Zustandsfelder und mehrere Dispatch-/Hilfsaufrufe; wahrscheinlich Objektzustands-Update. Funktionskoerper enthaelt Instruktionsueberlappungswarnung. **Niedrig-mittel**. |
| `0x83838-0x8b465`, 2.278 B | `FUN_00083838` | liest/schreibt `A0-A6,D0-D7,SP,Flags`; 2 Caller, 60 Calls, 22 Refs | Decompiler erkennt einen Switch ueber ein Byte aus `A6[2]` mit vielen case-Helfern; moeglicher Objekt-/Aktionsdispatcher. PCode warnt vor ueberlappenden/ungueltigen Instruktionen. **Niedrig-mittel**. |
| `0x839ea-0x8af93`, 3.088 B | `FUN_000839ea` | liest/schreibt fast alle `A0-A6,D0-D7` und Flags; 2 Caller, 80 Calls, 20 Refs | Switch ueber ein Byte bei `A6+1`, mehrere Case-Arme und Helferaufrufe; moeglicher zweiter Zustands-/Aktionsdispatcher. Decompiler meldet bad instruction data und Ueberlappung. **Niedrig-mittel**. |
| `0x869ae-0x869dd`, 48 B | `FUN_000869ae` | liest `A0-A2,D0w,D3w,D7b,D7w,SP`; schreibt `A1,A2,D3w,D7b,D7w,SP`; 18 Caller, 2 Refs | Vier wiederholte Eintraege werden mit Daten aus Tabelle `0x869de` aufgebaut; vermutete Grafik-/Tile-Descriptor-Initialisierung. **Niedrig-mittel**. |

## Nicht sicher zugeordnet

`FUN_00002540` hat 2.378 belegte Bytes ueber einen grossen Funktionsbereich, 57 Call-Sites und 75 Datenreferenzen. Der Decompiler warnt vor schlechter Instruktionslage, Ueberlappung bei `0x3812/0x3814`, unaufloesbaren Switches und instabiler Typfortpflanzung. Keine belastbare Rolle oder Hauptschleife zugewiesen; siehe [unresolved_functions.md](unresolved_functions.md).