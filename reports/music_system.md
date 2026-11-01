# Phase 15: Musiksystem

## Steuerung

`AudioEngine` unterstuetzt WAV-PCM16 laden, Musik starten, stoppen, pausieren und fortsetzen. `register_state_music(GameState, AudioHandle)` ordnet Tracks Spielzustaenden zu; `consume_gameplay_events` wechselt bei `StateChanged` automatisch. Pausieren haelt die aktuelle Position, Fortsetzen nimmt sie wieder auf. Derselbe Handle startet nicht erneut, wenn er bereits aktiv ist.

Standardrouting der neu erzeugten Platzhalter:

| Zustand | Musik |
|---|---|
| Title, CharacterSelect, GameOver | prozeduraler Title-Loop |
| FightIntro, Fighting | prozeduraler Fight-Loop |
| RoundResult, Victory, Defeat | prozeduraler Result-Loop |
| Paused | aktueller Track wird pausiert |

Die Platzhalterloops sind neu generierte Kurzphrasen; es gibt keine Originalmusik und keine Nutzungsrechtsbehauptung ueber fremde Medien.

## Pegel und Transport

Getrennte Master-, Effekt- und Musikbuslautstaerken liegen zwischen 0 und 1. Das Ausgabeformat des Mixers ist PCM16-Stereo mit standardmaessig 48 kHz. WAV-Dateien mit abweichender Sample-Rate werden beim Mischen per Nearest-Neighbor resampelt. Looping wird fuer Musik standardmaessig eingeschaltet.

`WavFileAudioOutput` schreibt den gemischten Stream als PCM16-RIFF/WAVE und finalisiert Groessenfelder beim Schliessen. Das ist eine dateibasierte Ausgabe, keine Live-Lautsprecher-/SDL-Ausgabe. Ein nativer Deviceadapter und Crossfade/Ueberblendung sind offen.
