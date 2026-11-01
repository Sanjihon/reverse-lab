# Street Fighter II: Gameplay-Architektur (statisch)

## Evidenzstufen

- **Direkt belegt:** MAME-ROM-/Adressmap, Ghidra-Instruktionen, aktuelle Funktions-/Xref-Datenbank und Ghidra-Decompiler-Ausgabe.
- **Wahrscheinlich:** Codeform/Offsets sprechen fuer eine Rolle, aber keine eindeutige semantische Verbindung zu einem Kaempfer oder Spielzustand.
- **Unbekannt:** kein belastbarer Call-/Datenreferenzpfad oder dynamischer Nachweis.

## Strukturuebersicht

```mermaid
flowchart TD
  V[68000 Vektor 1: PC 0x40e] --> R[Reset-Init FUN_0000040e]
  R --> C[Coin-Control 0x800030]
  R --> G[Grafik-RAM-Clear bei 0x73c]
  G --> J{JMP (A4), statisch zu 0x426}
  J -. undecoded .-> U[Reset-Folgepfad offen]
  E[Exception Vektoren] --> S[Stubs 0x400 / 0x402 / RTE 0x40c]
  T[TRAP Vektoren] --> H[A5-relative Record-/Listenhandler]
  H --> D[gemeinsamer Dispatcher 0xa5e]
  O[FUN_00083588] --> A[FUN_00001b1a: CPS/GFX init]
  O --> D1[FUN_00083838: switch ueber A6[2]]
  O --> D2[FUN_000839ea: switch ueber A6+1]
  D1 --> X[Objekt-/Case-Helper, Semantik offen]
  D2 --> X
  M[FUN_0007d6e2] --> Q[FUN_0007e25e: Collider-/Overlap-Kandidat]
  Q --> B[FUN_0007e486: X/Y-Overlap mit Facing-Korrektur]
  Q --> L[relative Dispatch-Tabelle 0x7e2f6]
  L --> S1[Handler 0x7e30a..0x7e3a6]
  S1 --> M2[Objektstatus-Update, Bedeutung offen]
  P[FUN_0007f1b4] --> W[Sprite-/Objektlisten-Emission, wahrscheinlich]
  I[MAME IN1/IN2 Bitbelegung] -. keine ROM-Leseroutine belegt .-> X
```

Das Diagramm zeigt direkte Call-Beziehungen nur dort, wo sie im Ghidra-Callinventar belegt sind. Die Verbindung von MAMEs Input-Portdefinition zu einem ROM-Leser fehlt. Der indirekte Registerdatenfluss fuehrt statisch zum dekodierten LEA-Einstieg `0x426`; `0x42a` verzweigt nach `0x7cc`. Das ist kein dynamischer Trace.

## Einstieg, Initialisierung und Exceptions

- Reset-PC aus Vektor 1: `0x0000040e`.
- `0x40e` schreibt an MAME Coin-Control `0x800030`, setzt A4 PC-relativ und verzweigt zu `0x73c`.
- `0x73c` loescht `0x900000-0x903fff`, danach folgt `JMP (A4)` bei `0x750`. Der statische Registerdatenfluss fuehrt zu `0x426`, einer zweiten LEA; `0x428` ist deren Extension-Word. `0x42a` verzweigt weiter nach `0x7cc`.
- `0x752` enthaelt einen Main-RAM-Clear ab `0xff0000`, aber seine Erreichbarkeit aus Reset ist nicht belegt.
- `0x400/0x402` setzen SP aus A5 und springen nach Reset; `0x40c` ist `RTE`; IPL2-Ziel `0xa94` ist undecodiert.
- TRAP-Handler `0xb24-0xd34` bearbeiten A5-relative Eintraege, Slots, Listen und Exception-Kontexte.

## Laufzeitnahe Subsystem-Kandidaten

### Objekt-/Collision-Pfad

`FUN_0007d6e2` ruft bei `0x7d702` `FUN_0007e25e` auf. Dieser waehlt optionalen Descriptorinhalt, vergleicht X/Y-Positionen und Extents ueber `FUN_0007e486`, berechnet eine Maske und dispatcht ueber `DAT_0007e2f6`. Die Handler `FUN_0007e30a`, `FUN_0007e318`, `FUN_0007e326`, `FUN_0007e34c`, `FUN_0007e35a`, `FUN_0007e380`, `FUN_0007e3a6` setzen A5-relative Kandidaten-/Achsenfelder. Der Pfad ist generische Ueberlappungs-/Objektzustandslogik; eine Attack/Hurt/Damage-Semantik ist nicht bewiesen.

### Grafik-/Objektaufbereitung

`FUN_00009314` ruft `FUN_00009476` und weitere GFX-/Layer-Helfer. `FUN_0007f1b4` verarbeitet Objekt-/Tile-Descriptoren, clipped Koordinaten und schreibt Vier-Wort-Ausgabe-Eintraege. `FUN_000869ae` initialisiert wiederholt Eintraege aus einer ROM-Tabelle. Diese Routinen sind Rendering-/Descriptor-Kandidaten, keine nachgewiesenen Hit- oder Angriffsabläufe.

### Zustand/Dispatch

`FUN_00083588` ruft `FUN_00083838` und `FUN_000839ea` auf; die beiden enthalten switch-artige Pfade ueber Bytes aus A6. `FUN_0007d6e2`/`FUN_0007d9b4` benutzen A5-/A2-/A3-relative Statusfelder. Die Namen der Zustandsautomaten, Fighter-IDs und Aufrufkadenz sind unbekannt. Es gibt keinen sicher isolierten Frame- oder Main-Loop.

## Spielerinput und Game-States

MAME `IN1` definiert active-low P1/P2-Richtungen und sechs Buttons; `IN2` deklariert zusaetzliche Kicks. Das ist Hardwarebelegung. In Ghidra fehlen absolute Xrefs zu den Input-/DIP-Portfenstern, und die 14 untersuchten UI-Stringobjekte haben null Xrefs. Daher sind Inputpolling, P1/P2-Weiterleitung, Titel-/Select-/Fight-/Round-/Win-Lose-Zustaende nicht mit ROM-Routinen verbunden.

## Datenstruktur-Kandidaten

- A5-relative Record-/Slot-/Listenstrukturen aus TRAP-Handlern.
- 256-Byte-Ringpuffer-Kandidat in `FUN_000021e2` (`A5+0xa2`, Index `A5+0x1e`).
- Collider-Descriptor-Kandidat an Objektfeldern `A2+0x1a`, `A2+0x34`, mit `A3/A4` Bounds und `A2/A6+0x12` Facingflags.
- Grafik-Descriptoren/Commandlisten mit unbekannten Typen und Einheiten.

Siehe `reports/REMAINING_QUESTIONS.md` fuer den priorisierten Restumfang.
