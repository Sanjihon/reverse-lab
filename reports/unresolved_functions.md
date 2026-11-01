# Nicht aufgeloeste Funktionen und Analysegrenzen

## Hauptschleife und dynamische Kontrolle

- Eine eindeutige Hauptprogrammschleife konnte nicht belegt werden. `FUN_000839ea` und `FUN_00083838` sind grosse switch-artige Dispatchfunktionen, aber keine von beiden ist als Frame-/Main-Loop nachgewiesen.
- Reset folgt bei `0x750` einem `JMP (A4)`. Der Xref der LEA bei `0x41e` zeigt auf `0x426`; A4 bleibt im GFX-Clearpfad unveraendert. Bei `0x426` beginnt eine zweite LEA, `0x428` ist deren Extension-Word, und `0x42a` verzweigt nach `0x7cc`. Das ist statische Registerdatenfluss-Evidenz, kein dynamischer Trace. Weitere `JMP (A4)` stehen bei `0x766` und `0x7ae`; deren indirekte Ziele bleiben unaufgeloest.
- Der Interrupt-Level-2-Vektor zeigt auf `0xa94`; Ghidra konnte dort keine Instruktion dekodieren.
- Das API-Inventar fand 636 direkte Call-Edges ohne unaufgeloestes direktes Call-Ziel, aber keine computed-call-Referenzen. Das ist keine Garantie, dass es keine indirekten Calls oder dynamischen Sprungziele gibt.

## Funktionale Zuordnung mit niedriger Konfidenz

| Adresse / Ghidra-Name | Grund der Unsicherheit |
|---|---|
| `0x00002540` / `FUN_00002540` | 2.378 belegte Bytes, 57 Calls, 75 Datenreferenzen; Decompiler meldet bad instruction data, Instruktionsueberlappung bei `0x3812/0x3814`, nicht normalisierte Switches und instabile Typfortpflanzung. Nicht als Hauptschleife oder Init benennen. |
| `0x000839ea` / `FUN_000839ea` | Switch-/State-Dispatcher mit 80 Calls und 20 Referenzen, aber bad instruction data und Ueberlappungswarnungen; Fallsemantik nicht stabil. |
| `0x00083838` / `FUN_00083838` | Zweiter switch-artiger Dispatcher mit 60 Calls und 22 Referenzen; Instruktionsueberlappung und unerreichbare Bloecke. |
| `0x0007d9b4` / `FUN_0007d9b4` | Zustands-/Objekt-Update-Kandidat mit 23 Calls und 18 Referenzen, aber Instruktionsueberlappung bei `0x7dac6/0x7dac8`. |
| `0x00003a72` / `FUN_00003a72` | Ghidra meldet 190 belegte Bytes bei einer Endadresse `0x77343`; extrem grosse Koerper-Spanne deutet auf unzuverlaessige Grenzen hin. |
| `0x00000a94` | Vektorziel IPL2, kein dekodierter Funktionskoerper im Listing. |
| `0x0000040e` / `FUN_0000040e` | Reset-Instruktionen sind im Listing lesbar; die Ghidra-Dekompilation spiegelt jedoch auffaellig den Code von `0x400` wider und ist fuer die Funktionssemantik nicht verlaesslich. |

## Offene globale Daten

- `A5`-relative Offsets bilden wahrscheinlich Runtime-Records, Pool-/Queue-Metadaten und Flags ab. Der absolute A5-Basiswert und die Feldtypen sind nicht aufgeloest.
- 975 Datenreferenzen wurden inventarisiert, aber nur 81 zeigen in GFX-RAM und 8 in CPS-I/O. Direkte statische Referenzen in MAME-Main-RAM wurden nicht gefunden; registerbasierte Zugriffe koennen dadurch unsichtbar bleiben.
- ROM-Ziele wie `0x9ca`, `0x9e0`, `0x96be` und `0x869de` werden als Tabellenkandidaten verwendet. Tabellenformat und semantische Namen sind nicht fuer alle bestimmt.
- Der Decompiler von `FUN_0007f1b4` legt eine Sprite-/Objektlisten-Emission nahe; `FUN_00009476` wirkt wie GFX-/Layer-Setup. Beide bleiben Hypothesen und wurden nicht umbenannt.

## Grenzen der Analyse

Ghidras Standard-Autoanalyse meldete 488 nicht-fatale PCode-Decompiler-Warnungen. Die ausgewahlte Language `68000:BE:32:default` ist Ghidras 68000-Default und dekodiert laut Provider-Evidence den 68040-Superset. Das kann zusammen mit ROM-Daten und alternativen Instruktions-Entries zu falschen Funktionsgrenzen, Ueberlappungen und unzuverlaessigen Switches fuehren. Es wurde statisch analysiert; CPU-Ausfuehrung, Interruptlaufzeit und Emulation fanden nicht statt. Funktionen wurden nicht umbenannt.