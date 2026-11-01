# Phase 13: Bewegungssystem

## Implementierung

`update_movement` in `implementation/src/player/movement_system.cpp` verarbeitet einen diskreten Input pro Simulationsschritt:

- Links/rechts setzt horizontale Geschwindigkeit; gleichzeitiges Druecken neutralisiert die Richtung.
- `run` waehlt die neu entwickelte Laufgeschwindigkeit, sonst wird Gehgeschwindigkeit verwendet.
- `jump` startet bei Bodenkontakt nur an der Tastendruckkante; Gravitation wird pro Tick addiert.
- Vertikalzustand wechselt zwischen `Jumping`, `Falling` und `Landing`; im naechsten Bodentick folgt `Standing` oder ein neuer Inputzustand.
- `crouch` stoppt horizontale Bewegung und setzt `Crouching`, solange Bodenkontakt besteht.
- Ohne Richtung, Sprung oder Ducken wird der Spieler `Standing`.
- X wird auf die Stagegrenzen begrenzt.

## Neu entwickelte Parameter

| Profil | Gehgeschwindigkeit | Laufgeschwindigkeit | Sprungimpuls Y | Gravitation/Tick |
|---|---:|---:|---:|---:|
| `TrainingBalanced` | 2.0 | 3.5 | -8.0 | 0.45 |
| `TrainingHeavy` | 1.7 | 2.8 | -7.2 | 0.48 |

Einheiten sind neue Weltkoordinaten pro 60-Hz-Simulationsschritt. Y waechst nach unten; negative Sprunggeschwindigkeit bewegt nach oben. Diese Parameter sind Prototypwerte, keine aus dem Original abgeleiteten Werte.

## Nicht enthalten

Beschleunigungskurven, Dash-/Spezialbewegungen, Crouch-Hitboxen, Trefferstop, Rueckstoss, Sprungarten, Bodenkollisionen mit Gegnern und exakte SF2-Framewerte fehlen. In den Originalbefunden wurde keine belastbare Bewegungsparameter-Tabelle verifiziert.
