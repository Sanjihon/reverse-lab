# Phase 18: Gameplay-Validierung

## Zustands- und Rundendurchlaeufe

Automatisiert geprueft: Title -> CharacterSelect -> FightIntro -> Fighting, Pause/Fortsetzen, RoundResult, P1-Sieg/P1-Niederlage, GameOver und Neustart. Das Best-of-three-Testmodell spielt zwei entschiedene Runden bis Victory/Defeat; Runde 2 erhaelt volle HP und behaelt Rundensiege.

Die Zustandsnamen und Rundenregeln sind neue Projektregeln. Es gibt keine gesicherte Zuordnung zu den unbekannten CPS1-State-/Dispatchfunktionen.

## Bewegung und Spieler

P1/P2 werden getrennt initialisiert und koennen gleichzeitig gesteuert werden. Tests decken Links/Rechts, Rennen, Springen, Ducken, Stehen, Landen, relative Blickrichtung, Bounds und beide `TrainingBalanced`-/`TrainingHeavy`-Profile ab. Ein 10.000-Simulationsschritt-Headlesslauf blieb im Fighting-State; Spielerkoordinaten blieben an der Stagegrenze.

## Combat-/Collision-Validierung

Die Testfolge bewegt P1 in Reichweite, erkennt die neue AABB-Attack-/Hurtbox-Ueberlappung, verarbeitet einen Heavy-Punch, reduziert P2-HP und beendet die Runde. Zusaetzliche Tests pruefen alle sechs neuen Punch-/Kick-Level und den Attack-Cooldown.

Die Werte fuer Boxen, Schaden und Cooldown sind im `CombatConfig` neu definierte Prototypewerte. Kein Test behauptet, dass sie originale SF2-Hitboxen oder Schadenswerte darstellen.

## Begrenzungen

Blocken, Guardrichtung, Knockback, Hitstun, Invulnerabilitaet, Attackprioritaet aus Originaldaten und Spezialmove-Dispatch sind nicht implementiert. Der Recognizer fuer Spezialbewegungen wird nicht durch Combat verwendet. Wenn P1 und P2 im selben Tick treffen koennten, wird aktuell nur der erste Treffer in P1/P2-Reihenfolge gemeldet. Diese Einschraenkungen sind bekannte fehlende Funktionalitaet, keine bestandenen Validierungen.
