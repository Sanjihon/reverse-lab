# Bewegungs- und Objektzustandskandidaten

Untersuchungsgrundlage: das gespeicherte Ghidra-Projekt `working/SF2MainCPU.gpr` und `working/inventory_final.log`/`working/phase6_decompiler_expanded.log`. Ghidra-Funktionen tragen weiterhin ihre automatisch erzeugten Namen. Keine im Folgenden genannte Routine wurde als Spielerbewegungsfunktion bewiesen.

## Kandidat `FUN_0007d6e2`

- Ghidra-Adresse: `0x0007d6e2`; unveraenderter Ghidra-Name `FUN_0007d6e2`.
- Kontrollfluss: prueft `A5+0xade` und `A5+0x2e3`; ruft `FUN_0007e3f6`, unter einer weiteren Bedingung `FUN_0007e25e`, setzt dann Status-/Objektfelder und ruft `FUN_0007d9b4`, einen Thunk bei `0x6314` sowie `FUN_0007dcd6` auf.
- Sichtbare Variablen: `A5+0xade`, `A5+0x2e3`; bei A2-relative Feldern `+0x151`, `+0x15a`, `+0x4c`, `+0x47`, `+0x54`, `+0x2a`, `+0x71`; A3 `+8`; A6 `+0x47` und `+0x12`.
- Begruendung: mehrere Null-/Statuspruefungen, Schreiben eines Zustandswerts `0x78`, Setzen eines Flags/Unterzustands und Aufruf einer grossen stateful Routine passen zu einer Objekt-/Animationszustandsueberfuehrung.
- Konfidenz: **mittel** fuer Zustandsupdate; **niedrig** fuer Spielerbewegung. Keine Koordinaten-/Speedsemantik oder Inputquelle ist belegt.

## Kandidat `FUN_0007d9b4`

- Ghidra-Adresse/Koerper: `0x0007d9b4-0x000849f9`, 1.098 belegte Bytes. Der weite Adressbereich ist durch disjunkte Ghidra-Koerper bedingt und kein einzelner linearer Codeblock.
- Kontrollfluss: liest Zustands-/Mode-Bytes bei A3/A2-relativen Strukturen, verzweigt nach Werten (u. a. `2`, `4`, `0x63`, `0xa7`) und ruft viele lokale Case-/Helperfunktionen auf; 23 direkte Calls, 1 eingehender Call laut Inventory.
- Sichtbare Variablen: `A2+0x31`, `A2+0x4b`, `A2+0x49`, `A2+0x181`; ein Pointer aus `A2+0x0d`; State-Byte bei `A3+7` und alternative State-Quelle bei `A3+0xb`; ausserdem `A5+0x...`-relative Kontextzugriffe.
- Begruendung: switch-/case-artige Zustandszweige und wiederholte lokale Handler passen zu einem Objekt- oder Animationszustandsupdate.
- Konfidenz: **niedrig-mittel**. Ghidra meldet Instruktionsueberlappung bei `0x7dac6/0x7dac8`; daher sind Case-Semantik und Grenzen nicht stabil. Keine Funktion wurde umbenannt.

## Kandidat `FUN_0007f1b4`

- Ghidra-Adresse: `0x0007f1b4-0x0007f8a3`, 1.244 belegte Bytes.
- Kontrollfluss: optionaler Flagpfad zu `FUN_0007f8a4`, anschliessend `FUN_0007f56a`, dann Verarbeitung einer Objekt-/Tile-Beschreibung. Der Code prueft ein Budget, liest Flags/Koordinaten, schneidet Werte gegen `0x200` ab und schreibt Vier-Wort-Eintraege in einen Ausgabepuffer.
- Sichtbare Variablen/Register: Objektzeiger ueber `A0/A1/A3/A6`; Felder relativ zu `A5+0x2b6`, `A5-0x7508`, `A5-0x7506`; Beschreibung bei `*(A1+0x1a)+4`; Calls zu `0x7f8a4`, `0x7f56a` und `0x7f62e`.
- Begruendung: Koordinatenclipping, flags und wiederholte Descriptor-Ausgabe passen zu Sprite-/Objektlisten-Emission.
- Konfidenz: **mittel** fuer Render-/Objektlistenlogik; **niedrig** als Bewegungsroutine. Es werden Positionswerte verarbeitet, aber keine Bewegungsgeschwindigkeit oder Richtungseingabe nachgewiesen.

## Kandidat `FUN_000021e2`

- Ghidra-Adresse: `0x000021e2-0x00002223`, 66 Bytes.
- Kontrollfluss/Variablen: A5-relative Guards; D0w/D1w werden an `A5+0xa2+Index` geschrieben, Index bei `A5+0x1e` mit `&0xfc` auf einen 256-Byte-Ringbereich begrenzt.
- Begruendung: deutliche Ringpuffermechanik; direkte Caller schliessen `FUN_00002540`, `FUN_0000356c` und weitere nahe Funktionen ein.
- Konfidenz: **hoch** fuer generischen Ringpuffer, **niedrig** fuer Eingabe-/Bewegungsbedeutung. Nicht als Controller-Buffer benannt.

## Feststellung

Keine Routine liefert eine belastbare Zuordnung fuer Joystick-Richtung zu X/Y-Position, Laufgeschwindigkeit, Sprunghoehe, Ducken, Beschleunigung oder fighter-spezifische Physik. Die P1/P2-Masken sind in MAMEs `IN1`-Port belegt, aber es gibt keine Ghidra-Code-Referenz auf dessen CPU-Adressfenster. `FUN_00009476` und `FUN_00009314` enthalten Grafik-/Layer-Arbeit und sind keine belegte Positionsbewegungslogik.