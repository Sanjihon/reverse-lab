# Phase 13: Grundlegende Spiellogik

## Umfang

Die neue, SDL-freie C++20-Core-Logik liegt ausschliesslich unter `implementation/`. Die Rekonstruktion unter `src/`, Original-ROMs, `working/maincpu.bin` und das Ghidra-Projekt wurden nicht veraendert.

Implementiert sind Sessioninitialisierung, Fixed-Step-Zeitakkumulation, Frameverarbeitung, Pause/Fortsetzen, Neustart, Charakterauswahl, Spielerzustand, grundlegende Bewegung, Schadensanwendung als extern aufrufbare Regel und Best-of-three-Rundenverwaltung. Im Produktionscode wurden **27 neue C++-Funktionsdefinitionen** angelegt; Test-Hilfsfunktionen sind nicht mitgezaehlt.

## Ablauf

- `GameSession` startet im Zustand `Title` mit zwei generischen Training-Charakteren.
- `GameLoop::advance(elapsed_seconds)` gibt bis zu acht Simulationsschritte mit 60 Hz und einen Interpolationswert zurueck. Ueberzaehlige Catch-up-Zeit wird verworfen, um eine Schleifenkaskade zu verhindern.
- Die aufrufende Anwendung ruft pro Schritt `GameSession::process_frame(input)` auf. Eine SDL-Fenster-/Ereignisschleife ist nicht enthalten.
- Schaden wird vorerst ueber `apply_damage(PlayerId, amount)` zugefuehrt. Bei null Lebensenergie wird eine Runde entschieden. Es gibt keine Attacke, Hitbox oder Schadensformel.
- Zwei Rundensiege beenden eine Best-of-three-Serie. Sieg/Niederlage sind aus Sicht von Spieler 1 ausgedrueckt.

## Neue statt verifizierte Regeln

Es existieren keine verifizierten Originalwerte fuer Spielerleben, Geh-/Lauftempo, Sprungimpuls, Gravitation, Stagegrenzen oder Rundenzahl. Die beiden Trainingsprofile in `character_definition.cpp` sind ausdruecklich **neu entwickelte Prototypwerte**. Es sind weder originale SF2-Fighter noch behauptete ROM-Daten.

`ActionState` wird als `Neutral` initialisiert. Attack-, Guard-, Hitstun- und Knockdown-Verhalten sind nicht implementiert, weil Move-Daten und Kollisionsbedeutung in der Rekonstruktion UNKNOWN/PROBABLE bleiben.

## Build

Headless build und Tests laufen ohne SDL:

```sh
cmake -S implementation -B implementation/build -DCMAKE_BUILD_TYPE=Debug
cmake --build implementation/build --parallel 2
ctest --test-dir implementation/build --output-on-failure
```

SDL3 bleibt fuer ein spaeteres Frontend geplant. Die lokale Umgebung hat keine SDL-Entwicklungsheader. Renderer, Audioausgabe, echte Tastatur-/Controllerabfrage und visuelle Screens sind nicht Teil dieses Pakets.
