# Phase 15: Soundeffekte

## Cue-Katalog

`AudioCue` bietet Platz fuer Hit, Punch, Kick, Jump, Landing, SpecialAttack, Block, RoundStart, RoundWin, RoundLoss, Victory und Defeat. Beim Engine-Start werden fuer alle Cues kurze, **prozedural neu erzeugte Sinus-Ton-Platzhalter** erzeugt; sie sind keine Samples aus den Original-ROMs.

| Gameplay-/Engine-Event | Cue |
|---|---|
| `JumpStarted` | Jump |
| `Landed` | Landing |
| `HitConfirmed` | Hit |
| `RoundStarted` | RoundStart |
| `RoundWon` / `RoundLost` | RoundWin / RoundLoss |
| State `Victory` / `Defeat` | Victory / Defeat |
| Combat-/Animation-Aufrufer | Punch, Kick, SpecialAttack, Block per `play_effect` |

Die letzten vier Cues sind verfuegbar, werden aber nicht automatisch aus einem Kampfereignis ausgeloest: Phase 13 hat noch keine Punch-/Kick-/Special-/Block-Kampfaktionen. Das Audio behauptet keine Attacke, die das Spielmodell noch nicht erzeugt.

## Kanäle und Pegel

Effektkanaele laufen parallel zur Musik. Standard sind acht Kanaele; bei Ueberlast wird ein Kanal round-robin ersetzt. Buspegel fuer Master, Effekte, Musik und pro-Aufruf-SFX werden auf 0..1 begrenzt. Die Mischung wird vor PCM16-Ausgabe geclippt. Monoquellen werden auf Stereo dupliziert, Stereoquellen bleiben getrennt.

## Inhalte und Herkunft

- **Identifizierte Originaldaten:** `sf2_09.rom` (Audio-CPU) und zwei OKI-Sample-ROMs, durch MAME-Definition/CRC identifiziert.
- **Dekodierte Originaldaten:** keine.
- **Neue Dateien in `implementation/assets/`:** keine.
- **Neue Platzhalter:** prozedurale Tonsequenzen im Speicher; Test-WAVs entstehen temporaer und werden geloescht.

Keine Samplebytes wurden aus ROMs extrahiert. Spaetere lizenzierte oder selbst erstellte WAVs koennen ueber `load_wav_pcm16` registriert werden.
