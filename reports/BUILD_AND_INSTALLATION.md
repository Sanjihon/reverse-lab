# Build and Installation

## Voraussetzungen

- CMake >= 3.20; getestet mit 3.28.3.
- C++20-Compiler; getestet mit GCC 13.3.0 auf Linux.
- Keine externen Bibliotheken fuer headless Tests erforderlich.
- SDL3 wurde als Zielplattformbibliothek festgelegt, ist lokal aber nicht installiert; SDL3-Header und Liveadapter werden in diesem Build nicht benoetigt oder gelinkt.

Es gibt keine Installations-/Paketierungsroutine und kein eigenstaendiges App-Executable. Das Projekt baut Bibliotheken und Testprogramme.

## Debug

Aus `implementation/`:

```sh
cmake --preset debug
cmake --build --preset debug --parallel 2
ctest --preset debug
```

## Release

```sh
cmake --preset release
cmake --build --preset release --parallel 2
ctest --preset release
```

Ausserdem getestete direkte Konfiguration:

```sh
cmake -S . -B build/debug -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build/debug --parallel 2
ctest --test-dir build/debug --output-on-failure
```

Buildausgaben liegen unter `implementation/build/`. CTest fuehrt Gameplay, Grafik, Audio, Input und Integration/Soak aus.

## Sanitizer

Auf dem aktuellen GCC/Linux-Host getestet:

```sh
cmake -S . -B build/sanitize -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON \
  -DCMAKE_CXX_FLAGS='-fsanitize=address,undefined -fno-omit-frame-pointer' \
  -DCMAKE_EXE_LINKER_FLAGS='-fsanitize=address,undefined' \
  -DCMAKE_SHARED_LINKER_FLAGS='-fsanitize=address,undefined'
cmake --build build/sanitize --parallel 2
ASAN_OPTIONS=detect_leaks=1 ctest --test-dir build/sanitize --output-on-failure
```

Keine Dependencies installieren, `sf2/` ROM-Dateien oder `working/maincpu.bin` als Buildquellen einbinden.
