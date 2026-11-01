# Final Implementation Report

## Neue Software

`implementation/` ist vom Original-ROM, Ghidra-Projekt und der partiellen C-Rekonstruktion unter `src/` getrennt. Es handelt sich um eine neue C++20-Implementierung mit generatorneutralen CMake Debug-/Release-Presets.

| Modul | Implementierter Umfang |
|---|---|
| Core / GameApplication | Fixed-step tick, Initialisierung, Framefolge, Fehlerstatus und Modulkoordination. |
| GameSession / Player Manager | Title, Charakterauswahl, FightIntro, Fighting, RoundResult, Victory/Defeat, GameOver, Pause, Neustart, HP und Rundenscore. |
| Input | Normalisierte Keyboard-/Gamepad-/Joystickevents, zwei Playerprofile, Bindings, Buffer, Achsen-/Deadzonekonfiguration und sequenzielle Motion-Erkennung. |
| Movement | Links/rechts, Run, Jump/Fall/Land, Crouch/Stand, Bounds und Facing. |
| Collision / Combat | Neue AABB-Attack-/Hurtbox-Pruefung, sechs Angriffseingaben, einfache Werte, Cooldown, HP-/Rundenaufruf. |
| Graphics / Animation | RGBA-Software-Framebuffer, Layer-/Sprite-Commands, P6-PPM, Platzhalter, Scaling/Letterboxing und tickbasierte Clips. |
| Audio | PCM16 WAV laden/ausgeben, prozedurale Cue-/Musikplatzhalter, Effektkanaele, Volumenbusse und Gameplay-Event-Routing. |

Es gibt keinen distributablen `main()`-Prozess. Der integrierte Laufzeitpfad wird durch `GameApplication` in automatisierten Tests ausgefuehrt. Graphics und Audio verwenden headless Software-/Null-/WAV-Backends; SDL3-Liveadapter fehlen, weil Entwicklungsdateien nicht verfuegbar waren.

## Neue Regeln und Ressourcengrenzen

Lebensenergie, Geh-/Lauftempo, Sprung-/Gravitationswerte, Stagegroessen, Collisionboxen, Schaden, Cooldown, Attackprioritaet und State-/Rundensemantik sind neue Prototypewerte. Sie sind nicht als originalverifiziert gekennzeichnet. Charakterdaten bestehen aus zwei generischen Trainingsprofilen. `implementation/assets/` enthaelt keine Dateien; Laufzeitcheckerboards/Tontests sind prozedural.

Keine Datei aus `sf2/`, `working/` oder der Ghidra-Rekonstruktion wird als Runtimeasset verwendet. Blocken, Knockback, Hitstun, echte Spezialattacken und originale SF2-Assets fehlen.
