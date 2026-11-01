# Phase 14: Grafik- und Animationstests

## Build/Test

Ausgefuehrt:

```sh
cmake --build implementation/build --parallel 2
ctest --test-dir implementation/build --output-on-failure
implementation/build/graphics_tests
```

Der GCC-13.3/CMake-3.28.3-Build endete ohne Compilerwarnungen. CTest bestand **2/2 Testziele**: Gameplay-Regressionssuite und Grafik/Animationssuite. Die Grafik/Animationssuite bestand **7/7 Testfaelle**:

1. P6-PPM laden, Pfadcache und Texturfreigabe.
2. Laufzeit-Checkerboard-Platzhalter.
3. Animation laden, MovementState-Wechsel, Frame-Dauer, Loop-/Non-Loop-Abschluss und direkter Clipwechsel.
4. Layerreihenfolge und Hintergrunddarstellung.
5. Horizontales Sprite-Flippen und Alpha-Blending.
6. Virtuelle Fensteraufloesung, Integer-Skalierung, Letterboxing, Resize und Close.
7. Refresh-Deadline, Verspaetung und ungueltige 0-Hz-Konfiguration.

Die bestehende Gameplay-Suite bestand ebenfalls erneut mit **14/14 Fällen**. Die Grafiktests verwenden temporaere PPM-/`.anim`-Testdateien und generierte Platzhalter; `implementation/assets/` bleibt leer.

## Grenzen

Die Tests validieren den CPU-Renderer und die virtuelle Headless-Fensteroberflaeche, kein natives Desktopfenster. SDL3-Entwicklungsdateien fehlen in der Umgebung; deshalb gibt es keinen SDL-Backend-Build, keine Anzeigehardware-/VSync-/Controller- oder Audio-Tests. Kein Original-ROM oder extrahiertes Originalbild wurde verwendet.
