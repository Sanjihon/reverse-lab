# Phase 13: Gameplay-Testbericht

## Build und Testlauf

Ausgefuehrt:

```sh
cmake -S implementation -B implementation/build -DCMAKE_BUILD_TYPE=Debug
cmake --build implementation/build --parallel 2
ctest --test-dir implementation/build --output-on-failure
implementation/build/gameplay_tests
```

Ergebnis: CMake konfigurierte erfolgreich, alle Core- und Testziele bauten mit GCC C++ 13.3/CMake 3.28.3 ohne Compilerwarnungen, CTest meldete **1/1 Testziel bestanden**. Der Testlauf meldete **14/14 Gameplay-Testfaelle bestanden**.

## Abgedeckte Faelle

1. Spielerinitialisierung und Startwerte.
2. Titel- und Charakterauswahlzustaende.
3. Charakterwechsel, Locks und Kampfvorbereitung.
4. Fixed-Step-Taktung, Interpolation, Catch-up-Limit und verworfene Zeit.
5. Links-/Rechtsbewegung und Laufgeschwindigkeit.
6. Ducken und Stehen.
7. Sprung, Fallen und Landen.
8. Stagegrenzen.
9. Blickrichtung relativ zum Gegner.
10. Pause und Fortsetzen ohne Bewegung im Pausezustand.
11. Rundensieg, Lebensreset und Fortbestand des Rundenscores.
12. Best-of-three, Sieg und GameOver.
13. Niederlage aus P1-Sicht.
14. Neustart aus einem laufenden Kampf.

## Aussagegrenze

Tests pruefen die neu definierten C++-Regeln. Sie vergleichen nicht mit einer ausgefuehrten 68000-CPU und beweisen keine SF2-Verhaltensgleichheit. Es gibt keine SDL-, Renderer-, Audio-, Controller- oder End-to-End-Spieltests, weil diese Adapter noch nicht implementiert sind und SDL-Entwicklungsheader lokal fehlen.
