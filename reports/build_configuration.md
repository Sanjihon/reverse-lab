# Phase 17: Build-Konfiguration

## Toolchain und Ziele

- CMake mindestens 3.20; getestet mit CMake 3.28.3.
- C++20; getestet mit GCC 13.3.0.
- Keine externen Bibliotheksabhaengigkeiten fuer den headless Build.
- `gameplay_core`, `graphics_core`, `audio_core` und `application_core` sind getrennte CMake-Libraries; `application_core` linkt die drei Subsystemlibraries und `gameplay_core`.
- CTest-Ziele: `gameplay_tests`, `graphics_tests`, `audio_tests`, `input_tests`, `integration_tests`.
- Debug- und Release-Build verwenden `-Wall -Wextra -Wpedantic` (MSVC `/W4 /permissive-`). Keine Third-party packages oder Original-ROMs sind Buildinputs.

## Reproduzierbare Presets

`implementation/CMakePresets.json` definiert generatorneutrale Debug-/Release-Presets. Aus `implementation/`:

```sh
cmake --preset debug
cmake --build --preset debug --parallel 2
ctest --preset debug
cmake --preset release
cmake --build --preset release --parallel 2
ctest --preset release
```

Buildausgaben liegen unter `implementation/build/debug/` und `implementation/build/release/`. `BUILD_TESTING=ON` in beiden Presets.

## Abhaengigkeiten und nicht gebauter Target

Der headless Build braucht nur Standard-C++20 und CMake. SDL3-Entwicklungsdateien fehlen in der Umgebung; daher ist kein SDL3-Frontend/OS-Executable in der Targetliste und kein SDL-Paket verlinkt. Native Fenster-/Audiogeraete-/Ereignisinitialisierung ist nicht getestet. Original-ROMs, `working/maincpu.bin` und Ghidra-Datenbank sind keine Buildinputs.

Es gibt noch kein distributable Spiel-Executable: Phase 17 baut Bibliotheken und Testprogramme, nicht eine Anwendung mit `main()`.
