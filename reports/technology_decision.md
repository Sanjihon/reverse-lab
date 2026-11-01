# Phase 12: Technologieentscheidung

## Entscheidung

- **Sprache:** C++20.
- **Grafik, Audio, Eingabe und Controller:** SDL3, ausschliesslich an der Plattformgrenze.
- **Build:** CMake.
- **Tests:** CTest mit zunaechst kleinen, abhaengigkeitsfreien Testprogrammen; externe Testframeworks sind fuer den Architekturstart nicht erforderlich.

Dies ist eine Entscheidung fuer eine neue, eigenstaendige Implementierung. Die evidenzbasierte C-Rekonstruktion bleibt unter `src/` und wird nicht in `implementation/` verschoben oder automatisch wiederverwendet.

## Abwaegungen

| Thema | Entscheidung | Begruendung / Grenze |
|---|---|---|
| C oder C++ | C++20 | RAII fuer SDL-Ressourcen, typisierte Zustands-/Ereignismodelle und einfache Datenkomposition sind fuer Engine, Spieler, Animation und Combat nuetzlich. Der Simulationskern bleibt frei von SDL und testbar. Die vorhandenen C-Dateien behalten ihre bisherige Bedeutung und werden nicht umgeschrieben. |
| SDL2 oder SDL3 | SDL3 | Fuer einen Greenfield-Start wird die aktuelle SDL-Hauptversion vorgesehen, statt neue Adapter an SDL2 zu binden. SDL3 deckt 2D-Renderer, Audio-Streams, Ereigniseingabe und Gamepad-Erkennung ab. SDL2 ist breiter in aelteren Umgebungen erprobt, bringt hier aber keinen sofortigen Buildvorteil: lokal ist nur SDL2-Laufzeit 2.30 installiert, ohne Entwicklungsheader oder pkg-config-Datei. SDL3 ist lokal ebenfalls noch nicht installiert. |
| Plattformen | Linux, Windows, macOS als Ziel | CMake plus SDL3 stellt eine gemeinsame Plattformschicht bereit. Konkrete CI-Runner, Paketformate und Mindestversionen sind noch festzulegen. |
| Grafik | SDL3 2D-Renderer | Fuer einen 2D-Fighter reicht ein Renderer mit Texturen, Rechtecken und Render-Snapshots. Kein eigener Grafiktreiber und keine 3D-Engine sind fuer die erste Architektur erforderlich. |
| Audio | SDL3 Audio API | Audioevents bleiben vom Combat-/State-Code getrennt. Falls spaeter ein Decoder/Mixer fuer lizenzierte komprimierte Assets noetig ist, wird ein Zusatzpaket separat begruendet; noch keine SDL_mixer-Abhaengigkeit. |
| Eingabe | SDL3 Events und Gamepad API | Keyboard und Controller werden auf dieselben logischen Aktionen abgebildet. Hotplug, Remapping und Deadzones gehoeren in den Inputadapter, nicht in die Simulationsregeln. |
| Testbarkeit | Plattformneutraler Core + CTest | Kampf-, Bewegung- und State-Logik sollen ohne Fenster, Audio oder Controller deterministisch testbar sein. Adapter erhalten separate Tests, soweit sie ohne Hardware laufen. |

## Lokal vorhandene Voraussetzungen

- GCC C++ `13.3.0`, `cc`, CMake `3.28.3` und pkg-config sind verfuegbar.
- Installiert ist `libsdl2-2.0-0` Version `2.30.0+dfsg-1ubuntu3.1` als Laufzeitbibliothek.
- SDL2-/SDL3-Entwicklungsheader und pkg-config-Metadaten wurden nicht gefunden; SDL3 fehlt.
- Es gibt keine bestehende Buildkonfiguration und keine externen C++-Bibliotheken im Projekt.
- Es wurde nichts installiert und noch kein CMake-Projekt angelegt.

## Abhaengigkeitsgrenze

`implementation/src/core`, `game`, `player`, `character`, `combat` und `animation` sollen nur Standard-C++ verwenden. SDL3 wird auf `graphics`, `audio` und `input` begrenzt. Tests fuer Core-Regeln linken nicht gegen SDL. Original-ROMs und daraus extrahierte Assets sind keine Implementierungsressourcen; `assets/` ist nur fuer neu erstellte oder nachweislich lizenzierte Inhalte vorgesehen.

Vor Implementierungsbeginn sind die Zielplattformen zu bestaetigen und ein SDL3-Entwicklungspaket fuer die Buildumgebungen bereitzustellen. Das ist keine Installation in Phase 12.
