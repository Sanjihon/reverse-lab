# Phase 15: Audio-Testbericht

## Build/Test

Ausgefuehrt:

```sh
cmake --build implementation/build --target audio_tests --parallel 2
ctest --test-dir implementation/build -R audio_tests --output-on-failure
implementation/build/audio_tests
```

Ergebnis: Audio-Core und Tests bauten mit GCC C++ 13.3/CMake 3.28.3; CTest bestand **1/1 Audio-Testziel**, und **8/8 Audio-Testfaelle** bestanden.

## Abdeckung

1. AudioEngine-Initialisierung, Kanalzahl und prozedurale Cue-Erzeugung.
2. WAV-PCM16-Mono laden, Pegelbusse anwenden und erwartete Mixsamples pruefen.
3. Gleichzeitige Effekte, Kanalersetzung und Clippinggrenzen.
4. Musik starten, pausieren, fortsetzen, muten, stoppen und Clipfreigabe.
5. GameSession-State-/Sprung-/Treffer-/Rundenevents an Musik und Effekte routen.
6. State-spezifische Musik, Pause und Fortsetzen.
7. Ressourcen-/Ausgabefreigabe beim Shutdown.
8. Gemischtes Audio als WAV schreiben, RIFF-Header pruefen und erneut laden.

Der Null-Sink prueft Initialisierung/Mischpuffer ohne Audiogeraet. Der WAV-Roundtrip erzeugt nur eine temporaere Datei. Die Tests dekodieren keine Original-Sound-ROMs.

## Einschränkungen

Kein SDL3-Devicebackend und kein Live-Hardwaretest, weil in diesem Environment SDL3-/Audio-Entwicklungsheader fehlen. Die AudioEngine ist integriert und erzeugt PCM, aber ueber den Null-Sink ist nichts hoerbar; mit `WavFileAudioOutput` kann der Stream extern angehoert werden. Original-ROMs wurden nur nach Manifest/CRC identifiziert und nicht geschrieben oder dekodiert.
