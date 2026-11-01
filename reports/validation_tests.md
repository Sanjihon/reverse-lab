# Phase 11: Validierungstests

## Ausgefuehrter Test

```sh
cc -std=c11 -Wall -Wextra -Werror -Isrc tests/collision_helpers_test.c -o /tmp/sf2_collision_helpers_test
/tmp/sf2_collision_helpers_test
```

Exit-Code: 0. Der Test enthaelt **30 Assertions** fuer sieben C-Funktionen: sechs reine Helper aus `src/game/collision.c` sowie `recon_FUN_0007e486_overlap_model`.

## Testfaelle

- 16-Bit-Addition/Subtraktion mit Wraparound, einschliesslich `0xffff + 1`, `0x7fff + 1` und `0 - 1`.
- Bitmuster nach s16 (`0x7fff`, `0x8000`, `0xffff`) und Vorzeichenerweiterung von Bytewerten `0x7f`, `0x80`, `0xff`.
- Big-Endian-Wortloads fuer `0x0000`, `0x7fff`, `0x8000`, `0xffff`; 16-Bit-Negation einschliesslich `INT16_MIN`.
- Zwei sich exakt an X-/Y-Grenzen beruehrende Kandidaten; erwartete Deltas sind null.
- Getrennte X- und Y-Trennungsfaelle. Der X-Fehler laesst y-/delta-Outputs unberuehrt; der Y-Fehler behaelt den vorher berechneten X-Delta und laesst Y-Delta unberuehrt.
- Spiegelungsflags kehren die horizontalen Offsets um; effektive Positionen 97 und 108 sowie X-Delta 3.
- gesetztes Descriptorbit prueft die `NEG.W`-Magnitude statt Descriptorbyte 0; `0xfe` erzeugt Offset -254 und effektive Positionen -154/-153.

Die erwarteten Werte wurden aus den beobachteten 68000-Schritten und modulo-2^16-Arithmetik abgeleitet. Dieser Hosttest vergleicht nicht gegen einen ausgefuehrten 68000-Interpreter und beweist keine Gleichheit fuer alle Eingaben.

## Nicht getestete Bereiche

- Reset-Prefix nicht ausgefuehrt: die C-Funktion schreibt direkt an MMIO-Adressen `0x800030` und `0x900000..0x903fff`; kein emulierter Bus/Memory-Adapter vorhanden.
- Kein Input-, Bewegungs-, Zustands-, Character-, Move-, Animations- oder Hurtbox-Test: entsprechende Funktionen/Strukturen sind nicht ausreichend rekonstruiert.
- Collision-Modell wurde mit Eingaben isoliert getestet, bleibt aber `PROBABLE` aufgrund fehlender ABI-, Pointerlaengen-, Feldsemantik- und dynamischer Gegenbelege.
- Erfolgreich isoliert getestete C-Funktionen: 7. Erfolgreich isoliert getestete Original-Ghidra-Funktionen: 0. Dynamisch validierte Funktionen: 0.
