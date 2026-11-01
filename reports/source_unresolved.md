# Phase 9: Nicht implementierte Funktionen und offene Zuordnungen

## Mapping-Zaehler

`reports/source_function_mapping.csv` enthaelt fuer alle aktuellen 353 Ghidra-Funktionsstarts eine C-Zuordnung oder eine explizite `UNKNOWN`-Zeile:

- 1 `CONFIRMED`: `FUN_0000040e` -> `recon_FUN_0000040e_prefix`, partieller Side-Effect-Prefix.
- 2 `PROBABLE`: Ghidra-Entries `FUN_0007e486` (12-Byte-Fragmenteinstieg) und `FUN_0007e492` (156-Byte-Vergleichskoerper) -> ein kombiniertes Host-Modell `recon_FUN_0007e486_overlap_model`.
- 350 `UNKNOWN`: kein C-Funktionskoerper angelegt.

Das bedeutet nicht, dass die 350 Routinen alle vollstaendig unerforscht waeren; es bedeutet, dass Phase 9 keine sichere 1:1-C-Rekonstruktion fuer sie beansprucht.

## Kernfunktionen ohne Implementierung

- **Main/Game Loop:** nicht sicher isoliert; der Resetpfad ist statisch von `JMP (A4)` bei `0x750` zum LEA-Einstieg `0x426` und Branch `0x42a -> 0x7cc` rueckfuehrbar, aber weder dynamisch verfolgt noch als Main-Loop identifiziert. Kein rekonstruierter originaler `main.c`/Game Loop.
- **Input Polling:** keine ROM-Routine mit belegtem Zugriff auf MAME `IN1`/DIP-Portfenster. `IN2`-Adresse ist nicht belegt. Keine `input.c`.
- **Player-/Character-State:** keine belastbare Player-/Character-Identitaet fuer A2/A5/A6-relative Strukturen. Keine `player.c`, `character_data.c`.
- **Movement/Animation:** `FUN_0007d6e2`/`FUN_0007d9b4` bleiben Objektzustandskandidaten; `FUN_0007f1b4` bleibt Sprite-/Descriptor-Ausgabe. Keine Geschwindigkeits-, Sprung- oder Animationsimplementierung.
- **Combat/Damage:** keine sichere HP-Variable, Schadensformel, Blocklogik, Hitstun, Knockback oder Round-Win-Routine. Keine `damage.c`.
- **Interrupts:** `interrupts.c` enthaelt nur die beobachteten Vektorwerte, keine Handlerimplementation ausser dem getrennt abgebildeten Reset-Stub.
- **Collision metadata:** der Overlappfad ist nur `PROBABLE` als generische Boundsroutine; konkrete Attack/Hurtbox-Klassen und Blob-/Table-Formate sind unbekannt.
- **UI/Game states:** 14 passende Stringobjekte hatten null Ghidra-Referenzen; keine Title/Select/Fight/Win-Lose State-Machine implementiert.

## Ghidra-Grenzen

Ghidra meldete 488 nichtfatale PCode-Decompiler-Warnungen. `FUN_0000040e` hat disjunkte Body-Ranges; Phase 10 loeste den statischen indirekten Sprungpfad auf `0x426` und Branch `0x42a -> 0x7cc` auf, aber nicht die vollstaendige Reset-/Main-Loop-Erreichbarkeit. `0x428` ist das Extension-Word der zweiten LEA. Die Kollisionsroutine umfasst getrennte Ghidra-Symbolstarts `0x7e486` und `0x7e492`; der Host-Wrapper fasst ihre beobachtete Kontrollflussstrecke zusammen. `FUN_00002540`, `FUN_0007d9b4`, `FUN_00083838`, `FUN_000839ea` und `FUN_00003a72` haben unzuverlaessige Grenzen/Instruktionsueberlappungswarnungen.

Es wurde keine 68000-Ausfuehrung, kein Emulatorstart, keine ROM-Runtime-Instrumentierung und keine echte Boardmessung durchgefuehrt. Der C-Syntaxcheck belegt nur, dass die vorhandenen partiellen Source-Dateien kompilierbare Syntax besitzen.