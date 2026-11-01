# Phase 19: Performanceanalyse

## Messung

Aktueller Build: Debug, GCC 13.3.0, CMake 3.28.3, Linux. Ausgefuehrt wurde `/usr/bin/time -v build/debug/integration_tests` aus `implementation/`.

- 10.000 fixed-step Gameplay-Ticks im Headless-Soak: **209 ms** (Test-internes Wall-Time).
- Gesamte Integrationssuite mit 9 Faellen: **0,38 s Wall-Time**, **0,37 s User CPU**, **8.668 KiB Peak RSS**.
- Maximal 0,00 s System-CPU laut Ausgabe, 0 major faults, 1.809 minor faults, 0 swaps.
- Dies ist eine Einzelmessung auf der aktuellen Maschine, kein plattformweiter Benchmark oder FPS-Wert.

## Profilabdeckung und Entscheidung

- **CPU/Simulation:** 10.000 headless Session-/Input-/Combat-/Animation-Ticks ohne Absturz. Keine Profiler-Attribution pro Funktion, daher keine gezielte Hotspotoptimierung.
- **Rendering:** im Soak deaktiviert. Der CPU-Renderer wird funktional getestet, aber Pixel throughput/FPS fuer 640x360 oder groessere Ausgaben wurde nicht gemessen.
- **Animation:** wenige kleine Clips pro Spieler; kein beobachteter Speicher-/CPU-Engpass.
- **Collision:** pro Tick wenige Rechteckvergleiche; kein grosses Paar-Suchproblem benchmarked.
- **Speicher/Ressourcen:** Peak RSS umfasst den gesamten Integrationstestprozess, nicht nur den Soakabschnitt. Die App laedt keine Dateien im Soak; Ressourcen bleiben aus den Placeholder-Initialisierungen konstant.
- **Audio:** PCM-Mixer wird funktional getestet, nicht gegen ein Live-Device oder mit langen Musikstreams profiliert.

**Optimierungsentscheidung:** Keine algorithmischen Performanceaenderungen vorgenommen, weil kein Engpass gemessen wurde. Die beiden Phase-19-Aenderungen beheben Input-/Lifecyclezustandsfehler und sind keine Performanceoptimierungen.
