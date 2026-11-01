# Phase 17: Integrationstestbericht

## Debug-/Release-Build

Die generatorneutralen CMake-Presets `debug` und `release` konfigurierten und kompilierten alle Libraries und fuenf Testziele. In beiden Konfigurationen bestanden **5/5 CTest-Ziele**. Debug- und Release-Preset wurden je mit GCC C++ 13.3/CMake 3.28.3 ausgefuehrt.

## Integrationstests

Das dedizierte `integration_tests`-Ziel bestand **5/5 Testfaelle**:

1. Application initialisiert Session, Renderer, Audio-Null-Sink und praesentiert einen Frame.
2. Tastaturinput startet Character Select/Kampf; Bewegung, Sprite-Frame, Rendering, Musik und Sprung-Audioevent sind durch die Application verbunden.
3. Eingabe bewegt P1 in Reichweite; Collision prueft Angriff gegen Hurtbox, Combat erzeugt Treffer, Session aktualisiert HP und Runde.
4. Rundenergebnis geht durch Best-of-three zu Sieg; Neustart setzt Match/Spieler zurueck.
5. Ungueltige Fenster-, Combat- und Audioausgabekonfiguration liefert nachvollziehbare Initialisierungsfehler.

Weitere, nicht integrierte Unit-Tests bestehen weiterhin: Gameplay 14, Grafik/Animation 7, Audio 8, Input 10. Ueber alle Suites sind es 44 Testfaelle in fuenf CTest-Zielen. Alle sind headless.

## Aussagegrenze

Diese Tests beweisen die neu entwickelte C++-Integration mit Platzhalterregeln, nicht Original-SF2-Verhaltensgleichheit. Kein natives SDL-Fenster, reales Audioausgabegeraet oder physischer Controller wurde verwendet. Combat ist absichtlich einfach und kann Originalprioritaeten, Hitstun, Guard oder simultane Trades nicht behaupten.
