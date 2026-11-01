# Phase 18: Funktionstestbericht

## Zusammenfassung

| Kennzahl | Ergebnis |
|---|---:|
| Ausgefuehrte Testfaelle | 47 |
| Bestanden | 47 |
| Fehlgeschlagen | 0 |
| Nicht ausgefuehrte automatisierte Testfaelle | 0 |
| CTest-Testziele | 5/5 bestanden |

Die 47 Faelle zaehlen die Gameplay-, Grafik-, Audio-, Input- und Integrationssuites: 14 + 7 + 8 + 10 + 8. Die fehlenden Spielfunktionen am Ende dieses Berichts sind keine als fehlgeschlagen verbuchten Tests; sie besitzen noch keine Implementierung und konnten nicht ausgefuehrt werden.

## Build/Regression

Die generatorneutralen CMake-Presets `debug` und `release` konfigurierten und bauten alle Targets mit GCC 13.3.0 und CMake 3.28.3. Alle fuenf CTest-Ziele bestanden in beiden Konfigurationen. Compileroptionen umfassen `-Wall -Wextra -Wpedantic`; keine Warnungen wurden im sauberen Build gemeldet.

## Getestete Bereiche

- Programm-/Engine-Initialisierung, Title, Charakterauswahl, FightIntro, Pause, Sieg/Niederlage, GameOver und Neustart.
- Bewegung beider Player, Laufzustand, Sprung/Landung, Stagegrenzen, Facing und beide Trainingsprofile.
- Sechs Attackeingaben, Collision-Hit, Cooldown, HP-Verlust, Rundenausgang und Best-of-three-Neustart.
- Sprite-Rendering, Layer, Platzhalter, Scaling/Letterboxing, Animationswechsel und Frame-Dauer.
- Audioinitialisierung, SFX/Musik, Pegel, gleichzeitige Effekte, Gameplay-Event-Routing, Ressourcenfreigabe und WAV-Roundtrip.
- Keyboard, P1/P2 parallel, Controller-/Joystick-Normalform, Hotplug, Puffer, Richtungssequenzen, Chords und Dispatch.
- Moduluebergreifender 10.000-Tick-Soaklauf ohne Grafik/Audio.

## Nicht vorhanden, deshalb nicht validiert

Blocken, Trefferstun, Knockback, Spezialangriffe/Move-Dispatch, Charakter-unterschiedliche Attacken, echtes SDL-/OS-Fenster, reale Controllerabfrage, Live-Audiodevice, VSync und ein startbares GUI-Executable sind nicht implementiert. Es gab daher keine Funktionstests fuer diese Funktionen.
