# Phase 18: Charaktertests

## Implementierte Profile

Es existieren zwei generische Trainingsprofile: `TrainingBalanced` und `TrainingHeavy`. Sie sind keine SF2-Rosterfiguren und besitzen weder Originalsprites noch Original-Moves.

| Profil | HP | Walk | Run | Jump impulse | Gravity/tick |
|---|---:|---:|---:|---:|---:|
| TrainingBalanced | 1000 | 2.0 | 3.5 | -8.0 | 0.45 |
| TrainingHeavy | 1100 | 1.7 | 2.8 | -7.2 | 0.48 |

Alle Parameter sind neue Prototypewerte. Tests pruefen initiale Character-Zuordnung, Auswahlwechsel, HP, profilabhaengige Gehbewegung/Sprungparameter, MovementStates, generische Animation und Attackinput.

## Charakterabhaengigkeit

Beide Profile verwenden denselben allgemeinen CombatConfig fuer Attackreichweite, Schaden und Cooldown. Es gibt keine charakterindividuellen Attacken, Spezialmoves, Animationframes, Hit-/Hurtboxen oder Audio-Cues. Character-spezifische Angriffe und Spezialbewegungen wurden nicht getestet, weil sie nicht implementiert sind.

Weitere SF2-Charaktere: **nicht implementiert**. Die Phase-1-bis-11-Rekonstruktion belegt keine vollstaendige Charakter-/Move-Datenbank, daher wurden keine Originalcharaktere oder Werte nachgebildet.
