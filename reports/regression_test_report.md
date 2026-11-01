# Phase 19: Regressionstestbericht

## Endstand

| Konfiguration | CTest-Ziele | Ergebnis |
|---|---:|---|
| Debug | 5 | 5/5 bestanden |
| Release | 5 | 5/5 bestanden |
| ASan + UBSan + LeakSanitizer | 5 | 5/5 bestanden, keine Meldungen |

Die fuenf Ziele sind Gameplay, Grafik/Animation, Audio, Input und Systemintegration. Zusammen enthalten sie **49 Testfaelle**: 14 Gameplay + 7 Grafik + 8 Audio + 11 Input + 9 Integration. Diese Suite wurde in jeder Konfiguration ausgefuehrt.

## Relevante Regressionen nach Bugfixes

- Nach Controllerbuffer-Fix: Input-Suite 11/11 bestanden.
- Nach App-/Combat-/Input-Lifecycle-Reset: Integration-Suite 9/9 bestanden.
- Danach vollstaendige Debug-/Release- und Sanitizer-Matrix erfolgreich.

Fehler-/Nichtausgefuehrt-Zaehler fuer die vorhandenen automatisierten Testfaelle: **0 fehlgeschlagen**, **0 ausgelassen**. Nicht implementierte Produktfunktionen (Block, Knockback, native SDL, Langzeitstunden) sind in `known_bugs.md` als offene Funktionen aufgefuehrt und nicht als Testfaelle gezaehlt.
