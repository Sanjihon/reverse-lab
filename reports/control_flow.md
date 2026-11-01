# Kontrollfluss- und Zustandsanalyse

Grundlage: persistentes Ghidra-Projekt `working/SF2MainCPU.gpr`; `maincpu.bin` ist bei `0x0` geladen. Die Reset- und Vektorbytes stammen aus dem Image; Instruktionen stammen aus dem Ghidra-Listing `68000:BE:32:default`. Die hier genannten 345 Funktionen und 636 funktionsgebundenen direkten Call-Edges sind der Phase-5-Snapshot vor Anlage der acht Kollisions-Tabellenhandler. Der finale Phase-8-Stand umfasst 353 Funktionen und 11.356 ReferenceManager-Kanten; die aktuellen Zaehler und vollstaendigen Kanten stehen in `reports/COMPLETE_FUNCTION_CATALOG.md` und `working/final_reference_inventory.log`. Das Inventarskript fand keine computed-call-Referenz in Funktionskoerpern; indirekte Spruenge bleiben dennoch ein expliziter Vollstaendigkeitsvorbehalt.

## Reset und Speicherinitialisierung

1. Vektor 1 liefert Reset-PC `0x40e`.
2. `0x40e` schreibt `0x80`, danach `0`, an MAME-Coin-Control `0x800030`; die LEA bei `0x41e` setzt A4 per Ghidra-Xref auf `0x426`; `BRA.W` springt zu `0x73c`.
3. `0x73c` setzt A0=`0x900000`, A1=`0x903fff` und schreibt in einer Schleife Nullworte in GFX-RAM. Danach folgt `JMP (A4)`.
4. Der indirekte Sprung bei `0x750` ist per Registerdatenfluss auf den dekodierten Instruktionsstart `0x426` rueckfuehrbar. Dort beginnt eine zweite LEA; `0x428` ist ihr Extension-Word. `0x42a` verzweigt danach nach `0x7cc`. Diese Kette ist statische Evidenz, kein dynamischer Reset-Trace.
5. Bei `0x752` liegt ein separater Codeblock, der ab `0xff0000` mit 8.192 Schleifeniterationen zwei Longwords Null schreibt und damit `0xff0000-0xffffff` loescht. Der Block liegt hinter `JMP (A4)` bei `0x750`; seine Erreichbarkeit aus dem Resetpfad ist nicht bewiesen.
6. `0x7b0` beginnt einen weiteren Block ab GFX-RAM `0x910000`; `0x7cc` laedt Tabellenwerte PC-relativ und schreibt transformierte Longwords nach `0x900400`. Das ist ein belegter GFX-Initialisierungskandidat, aber sein Ausloeser ist nicht durch einen vollstaendigen Reset-Trace verifiziert.

## Exception-, Interrupt- und TRAP-Kontrollfluss

| Vektorgruppe | Ziel | Listingbefund |
|---|---:|---|
| Bus-/Address-Error (2-3) | `0x400` | `ADDQ.L #8,SP`; danach gemeinsamer Eintrag bei `0x402` |
| Illegal/Div0/CHK/TRAPV/Privilege/Trace/Line-A/Line-F (4-11) | `0x402` | `LEA (0,A5),SP`; `JMP 0x40e`, also Ruecksetzen auf Resetpfad |
| Spurious, IPL1, IPL3-7 (24-25,27-31) | `0x40c` | `RTE` |
| IPL2 (26) | `0xa94` | Zielbytes sind im Ghidra-Listing nicht dekodiert; offen |
| TRAP #0-#15 (32-47) | siehe Vektortabelle | Ziele fuehren in A5-relative Record-/Listenoperationen; TRAP #3 Handler verzweigt zum gemeinsamen Dispatcher `0xa5e` |

TRAP #0 bei `0xb24` initialisiert einen indizierten Record; TRAP #2 bei `0xbe4` loescht Felder; TRAP #3 bei `0xcc4` speichert USP/Exception-Frame und dispatcht ueber `0xa5e`; TRAP #8 bei `0xc12` durchsucht acht Slots und legt einen freigegebenen Pointer in einer Liste ab. `0xcb8` ist ein vielfach aufgerufener Wrapper: MOVEM-Sicherung, `TRAP #3`, MOVEM-Wiederherstellung, RTS.

## Calls, Dispatcher und Schleifen

- Ghidra erfasste 636 direkte Call-Edges. Alle erfassten direkten Call-Edges zeigen auf eine bekannte Funktion; das schliesst indirekte `JSR/JMP`-Ziele nicht aus.
- Der Resetpfad benutzt `JMP (A4)` bei `0x750`; weitere indirekte Spruenge stehen bei `0x766` und `0x7ae`. Fuer diese Spruenge ist kein vollstaendiger Zielgraph gesichert.
- `FUN_000839ea` enthaelt einen switch-artigen Dispatcher ueber ein Byte bei `A6+1`, 80 direkte Calls und 20 Datenreferenzen. `FUN_00083838` dispatcht ueber ein Byte bei `A6[2]`, mit 60 Calls und 22 Datenreferenzen. Beide sind plausible Zustands-/Aktionsautomaten; konkrete Zustandsnamen fehlen.
- `FUN_0007d9b4` enthaelt zustandsabhaengige Zweige und 23 Calls, aber ebenfalls eine Instruktionsueberlappungswarnung.
- `FUN_00002540` ist wegen massivem Kontrollfluss-/Instruktionsfehler kein verlaesslicher Hauptschleifenkandidat.
- **Hauptprogrammschleife:** nicht sicher isoliert. Die Reset-Initialisierung endet in einem nicht dekodierten indirekten Einstieg; die grossen Dispatchfunktionen sind nur Kandidaten.

## Sprungtabellen

Der Decompiler erkennt Switch-Tabellen/Case-Ziele bei `FUN_000839ea` (`switchD_000839f4`) und `FUN_00083838` (`switchD_00083842`). Fuer `FUN_00002540` erscheinen weitere Switch-Hinweise, aber der Decompiler verwirft Ziele wegen bad instruction data und fehlender Normalisierung. Diese Hinweise belegen Dispatch-Strukturen, nicht deren vollstaendige Case-Tabellen oder semantische Namen.