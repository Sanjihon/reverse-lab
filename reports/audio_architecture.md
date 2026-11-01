# Phase 15: Audioarchitektur

## Komponenten

```text
GameSession event queue
       |
       v
AudioEngine.consume_gameplay_events
       |                         |
       v                         v
AudioCue -> effect channels   GameState -> music track
       \                         /
        \---- PCM mixer --------/
                    |
          +---------+----------+
          |                    |
          v                    v
   NullAudioOutput      WavFileAudioOutput
   tests/inspection      offline PCM16 WAV
```

- `AudioResourceManager` besitzt WAV-Clips und prozedural erzeugte Platzhalter mit eindeutigen Handles.
- `AudioEngine` initialisiert 48-kHz-Stereo, Effektkanaele, SFX-/Musikbusse und Masterlautstaerke. Der Mixer mischt mehrere Stimmen, resampelt Quellen mit einfacher Nearest-Neighbor-Positionierung und clippt auf signed PCM16.
- `NullAudioOutput` erfasst Audioframes fuer headless Tests. `WavFileAudioOutput` schreibt PCM16-RIFF/WAVE-Dateien fuer offline abspielbare Ausgaben.
- `consume_gameplay_events` nimmt Events aus `GameSession::drain_events()`: Statewechsel steuern Hintergrundmusik; Sprung/Landung/Treffer/Rundenstart/-ausgang loesen SFX aus.

## Ressourcen-/Devicegrenze

Unterstuetzt werden unkomprimierte PCM16-WAV-Dateien mit 1 oder 2 Kanaelen sowie prozedurale Platzhalter. Audioassets werden per RAII gehalten und per Handle entladen; `unload_clip` stoppt referenzierende Stimmen und entfernt Cue-/State-Mappings.

Es gibt keinen SDL3-/ALSA-/Pulse-Entwicklungsheader im Environment. Deshalb ist kein Live-Device-Backend eingebaut. Die Ausgabeziele sind ein testbarer Null-Sink und eine WAV-Datei; ein spaeterer SDL3-Adapter passt an `AudioOutput`, ohne den Mixer oder die Spielregeln zu aendern. `aplay`/`paplay` sind nur Hostprogramme und werden nicht als Engine-Abhaengigkeit gestartet.

## Originaldatenabgrenzung

MAME identifiziert `sf2_09.rom` als Audio-CPU-ROM und `sf2_18.rom`/`sf2_19.rom` als OKI-Sample-ROMs. Lokal wurden Dateigroesse, MAME-Zuordnung und CRC validiert. Der Audio-CPU-Inhalt oder die OKI-ADPCM-Samples wurden in diesen Phasen **nicht dekodiert**. Die Engine liest keine ROM-Datei.
