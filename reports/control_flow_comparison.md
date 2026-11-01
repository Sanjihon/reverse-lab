# Phase 11: Kontrollflussvergleich

## Reset-Folge

Aus `working/phase9_source_evidence.log` und `working/phase10_source_evidence.log`:

```text
0x40e  MOVE.B #0x80,(0x800030).L
0x416  MOVE.B #0,(0x800030).L
0x41e  LEA (0x6,PC),A4          ; ReferenceManager DATA-Xref -> 0x426
0x422  BRA.W 0x73c
0x73c  LEA (0x900000).L,A0
0x742  LEA (0x903fff).L,A1
0x748  MOVE.W #0,(A0)+
0x74c  CMPA.L A1,A0
0x74e  BLS 0x748
0x750  JMP (A4)
0x426  LEA (0x6,PC),A4
0x42a  BRA.W 0x7cc
```

Der eingehende indirekte Sprung bei `0x750` fuehrt statisch an den bereits dekodierten Instruktionsstart `0x426`; die dortige LEA ersetzt A4, anschliessend verzweigt die Instruktion bei `0x42a` nach `0x7cc`. `0x428` ist das Extension-Word innerhalb der zweiten LEA, kein Instruktionsstart. Der Decompiler stellt die indirekte Verbindung nicht als Call-/Flow-Kante her; diese Kette kombiniert einen ReferenceManager-Xref, Instruktionsdekodierung und Registerdatenfluss und bleibt statische Analyse.

`recon_FUN_0000040e_prefix` stellt die beiden Bytewrites und den Clear-Loop dar. Es laesst LEA/BRA/JMP und den weiteren Initialisierungspfad weg. Daher stimmen Schleifenkoerper und MMIO-Seiteneffekte, nicht der vollstaendige Reset-Control-Flow. Das C-Prefix ist bewusst partiell und nicht ausfuehrbar gegen normalen Hostspeicher.

## Collision-Kontrollfluss

- `0x7e48c: BPL 0x7e492` fuehrt bei nicht gesetztem Vorzeichenbit in den zweiten Ghidra-Funktionsstart. Der gesetzte Vorzeichenbitpfad fuehrt ueber `NEG.W` bei `0x7e48e` und `BRA` bei `0x7e490` zu `0x7e498`.
- Der Comparator prueft zuerst X. `BHI` bei `0x7e4d8` springt bei nicht ueberlappender X-Achse nach `0x7e52a`, setzt D4 auf 0 und kehrt zurueck. Sonst wird X-Delta angepasst und der Y-Pfad ausgefuehrt.
- `BHI` bei `0x7e51a` nimmt den entsprechenden Y-Fehlerpfad nach `0x7e52a`. Bei Erfolg setzt `MOVEQ #1,D4` bei `0x7e526` das Erfolgsbit, danach folgen RTS bei `0x7e528`/`0x7e52c`.
- `recon_FUN_0007e486_overlap_model` drueckt dieselben sequenziellen X-/Y-Abbrueche als `return false` und den Erfolgsfall als `return true` aus. Die modellierten Tests pruefen fruehen X-Abbruch, spaeten Y-Abbruch und den Erfolgsweg.

## Calls und Zustandsuebergaenge

Ghidra-Referenzen zeigen vier direkte Calls von `FUN_0007e25e` an `0x7e270`, `0x7e292`, `0x7e2b4`, `0x7e2e6` zum Start `0x7e486`. Weitere direkte Calls zum Vergleichsstart `0x7e492` stehen bei `0x7e570`, `0x7e8b0` und `0x7e8c8`. Diese Kanten sind statisch nachgewiesen; ihre dynamische Ausfuehrungsreihenfolge und Spielzustandsbedeutung ist nicht validiert.

Im vorhandenen C gibt es keine Player-State-Machine, Bewegungsberechnung, Eingabeverarbeitung oder Rundenlogik. Eine Zustandsuebergangsvalidierung ist deshalb fuer diese Bereiche nicht moeglich und wurde nicht behauptet. Keine dynamische Rueckkehrpfad-/Registervalidierung wurde durchgefuehrt.
