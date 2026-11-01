# Phase 11: Verhaltensvalidierung

Stand: 2026-11-01. Verglichen wurden ausschliesslich die ausreichend belegten C-Modelle in `src/` mit dem gespeicherten Ghidra-Projekt, `working/maincpu.bin` und den MAME-Adressdefinitionen. Der erwartete Ordner `reconstruction/` ist nicht vorhanden. Die Phase-10-Freigabe gilt fuer Phase 11; Phase 12 wurde nicht begonnen.

## Ergebnis

| Kennzahl | Anzahl | Zaehldefinition |
|---|---:|---|
| Ghidra-Funktionen insgesamt | 353 | Aktueller Read-only-Ghidra-Export. |
| Statisch verglichene adressgebundene C-Rekonstruktionen | 2 | Reset-Prefix und Collision-Hostmodell. |
| Statisch verglichene Ghidra-Funktionsstarts | 3 | `0x40e`, `0x7e486`, `0x7e492`; der Resetstart enthaelt disjunkte Body-Ranges. |
| CONFIRMED | 1 | `FUN_0000040e`, aber nur fuer die in C dargestellten MMIO-/Clear-Side-Effects. Nicht modellierter Rest bleibt UNKNOWN. |
| PROBABLE | 2 | `FUN_0007e486` und `FUN_0007e492`; zwei Ghidra-Starts, ein gemeinsames Hostmodell ohne wiederhergestellte 68000-ABI. |
| UNKNOWN | 350 | Kein ausreichend validiertes C-Gegenstueck. 62 sind in frueheren Berichten erwaehnt, 288 haben keinen thematischen Berichtstreffer. |
| Isoliert getestete C-Funktionen | 7 | Sechs arithmetische Helper plus `recon_FUN_0007e486_overlap_model`; 30 Assertions bestanden. |
| Isoliert getestete Ghidra-Funktionen | 0 | Kein Test fuehrt den originalen 68000-Code aus. |
| Dynamisch validierte Funktionen | 0 | Kein Emulator, keine 68000-Ausfuehrung und kein Board-Runtime-Test. |

Die sieben isoliert getesteten C-Funktionen sind keine sieben bestaetigten ROM-Funktionen: sechs sind lokale Rechenhelper ohne 1:1-Ghidra-Entry; das siebte ist ein PROBABLE-Hostmodell.

## Funktionsstatus

- `FUN_0000040e` / `recon_FUN_0000040e_prefix`: **CONFIRMED fuer den dargestellten Prefix**. Bytewrites an `0x800030` und der Wort-Clear an `0x900000..0x903ffe` stimmen mit den dekodierten Instruktionen ueberein. Die Funktion ist nicht vollstaendig rekonstruiert; ihr restlicher Kontrollfluss ist nicht Teil dieses Status.
- `FUN_0007e486` / `FUN_0007e492` / `recon_FUN_0007e486_overlap_model`: **PROBABLE**. Wortbreiten, Vorzeichenerweiterungen, Spiegelung, Achsenbedingungen und fruehe Rueckkehrpfade stimmen fuer die geprueften Pfade. C-Parameter, Outputpointer und `bool`-Rueckgabe sind Adapter, keine rekonstruierte 68000-ABI.
- Die uebrigen 350 Ghidra-Funktionen: **UNKNOWN** fuer Verhaltensgleichheit. Eine Decompiler-Ausgabe, Funktionsnennung oder Berichtserwaehnung ist kein C-Vergleich.

## Validierungsarten

- **Statisch verglichen:** die drei oben genannten Ghidra-Starts anhand gespeicherter Instruktionen, Decompilertexte, ReferenceManager-Kanten und C-Quelltext.
- **Isoliert getestet:** sieben C-Funktionen im Hostprozess mit kontrollierten Eingaben.
- **Dynamisch validiert:** keine.
- **Nicht validiert:** alle 353 originalen Ghidra-Funktionen hinsichtlich Laufzeitgleichheit; insbesondere reset-seitige Folgezustaende, echter Input, Character-/Move-Daten und Kollisionsbedeutung.

## Abweichungen und Einschraenkungen

1. Der Collision-Wrapper gibt in C `bool` und mehrere Outputpointer zurueck; Ghidra zeigt zwei getrennte `void`-Funktionsobjekte. Der 68000-Code schreibt Koordinaten in A5-relative Felder, Delta-Werte nach D2/D3 und setzt D4 auf 0/1. Die C-Schnittstelle bildet diese beobachteten Werte fuer Tests ab, nicht den Registervertrag.
2. Die C-Resetfunktion bildet Side Effects nach, nicht `BRA`, `JMP (A4)`, Registerzustand oder Rueckkehrpfad. Das ist eine deklarierte Coverage-Grenze, keine behauptete Kontrollflussgleichheit.
3. Die C-Laderoutine liest Big-Endian-Worte byteweise und toleriert ungerade Hostpointer. Der originale 68000-Wortzugriff erfordert Wortausrichtung und kann bei ungerader Adresse einen Address Error ausloesen. Die tatsaechliche Ausrichtung der Objektbasen ist nicht bewiesen.
4. Kein Vergleich wurde dynamisch gegen die Ausfuehrung des Originalprogramms durchgefuehrt. Testergebnisse belegen nur das definierte Host-C-Verhalten fuer die angegebenen Testdaten.

Die alte Phase-9-Zielangabe `0x428` ist in Phase 10 anhand des Ghidra-Xrefs korrigiert worden: die LEA bei `0x41e` zeigt auf `0x426`; `0x428` ist das Extension-Word der zweiten LEA bei `0x426`. Dieser Befund ist statisch, kein Runtime-Trace.
