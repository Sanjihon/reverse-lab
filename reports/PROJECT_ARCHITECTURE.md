# Project Architecture

## Zwei getrennte Artefaktbereiche

```text
sf2/ + MAME-Definitionen
          |
          v
working/maincpu.bin --> Ghidra-Projekt --> reports/ + source_function_mapping.csv
                                      \
                                       -> src/ (partielle, adressgebundene C-Rekonstruktion)

implementation/ (unabhaengige C++20-Software; keine ROM-/Ghidra-Runtimeabhaengigkeit)
          |
          v
GameApplication fixed-step tick
  -> InputManager -> GameSession/PlayerManager/Movement
  -> CollisionSystem/CombatSystem -> HP/Rundenergebnis
  -> AnimationController + GameplayEventQueue -> AudioEngine
  -> SpriteDrawCommands -> SoftwareRenderer -> virtueller WindowManager
```

Das Diagramm beschreibt Implementierungsverbindungen, nicht eine rekonstruierte Original-Spielschleife.

## Pakete

- `src/`: kleine C-Rekonstruktion aus Phasen 9-11; zwei adressgebundene C-Modelle und dokumentierte Hilfen.
- `working/`: persistentes Ghidra-Projekt, 1-MiB-Abbild und Evidenzlogs.
- `reports/`: phasenbezogene Evidenz- und Umsetzungsberichte.
- `implementation/`: neuer C++20-Code; CMake-Libraries `gameplay_core`, `graphics_core`, `audio_core`, `application_core` sowie Tests.
- `sf2/`: schreibgeschuetzte Analyse-ROMs, nicht in den Anwendungsbuild eingebunden.

## Laufzeitgrenze

`GameApplication::tick` nimmt normalisierte Eingabeereignisse und vergangene Zeit entgegen, fuehrt feste Simulationsticks aus, routet Sessionereignisse zu Audio/Animation und rendert in den CPU-Framebuffer. `WindowManager`, `NullAudioOutput` und `WavFileAudioOutput` sind headless/testbare Adapter. Native SDL3-Polling-/Window-/Audioimplementierungen fehlen. Kein standalone `main()`-Programm ist vorhanden.

## Evidenzstufen

Originalfunktionen und bekannte Speicheradressen sind in `FINAL_RECONSTRUCTION_REPORT.md` erfasst. Alle Prototype-Combat-, HP-, Bewegung-, Zeit- und Statewerte unter `implementation/` sind neue Regeln. Unbekannte Originalbereiche bleiben UNKNOWN.
