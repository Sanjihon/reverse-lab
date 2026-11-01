# Phase 14: Grafiksystem

## Implementiert

- `SoftwareRenderer` zeichnet in einen plattformneutralen RGBA8-`FrameBuffer`.
- `RenderLayer` ordnet Background, Stage, Characters, Effects und UserInterface. Innerhalb einer Ebene sortiert `order`; gleiche Werte bewahren die Eingabereihenfolge.
- Sprite-Kommandos unterstuetzen Quellrechtecke (Spritesheets), Zielrechtecke, horizontales Flippen und Alpha-Blending.
- Hintergrundfarbe und vollflaechige Bildkommandos koennen gemeinsam gerendert werden.
- `WindowManager` verwaltet im aktuellen Build eine **virtuelle Headless-Ausgabeflaeche**, nicht ein natives OS-Fenster. Er berechnet Fit-/Integer-Skalierung, Letterboxing und praesentiert den logischen Framebuffer auf die konfigurierte Aufloesung.
- `FramePacer` plant Frame-Deadlines anhand einer konfigurierbaren Hz-Rate. Ein Frontend kann bis zur Deadline warten; der Core schlaeft nicht selbst und konfiguriert keine Hardware-VSync.

Default-Vorschlag: logische 640x360-Aufloesung, virtuelle Ausgabe 1280x720, ganzzahlige 2x-Skalierung, 60-Hz-Pacing. Das sind neue Darstellungsparameter, keine rekonstruierten Originalwerte.

## Ressourcen und Assets

`ResourceManager` laedt unkomprimierte binaere PPM-P6-Texturen (RGB, Maxwert 255), cached identische Pfade und laesst Texturen/Animationen explizit oder gesammelt freigeben. `.anim`-Textdateien definieren Looping und Quellframes. Hintergrundbilder und Sprites verwenden denselben Texturpfad.

Es liegen keine Grafiken in `implementation/assets/`. Tests erzeugen temporaere PPM-Daten und Laufzeit-Platzhalter mit kontrastierendem Checkerboard. Keine Original-ROMs oder daraus extrahierten Grafiken werden geladen oder kopiert.

## SDL-Grenze und Einschraenkung

Phase 12 hat SDL3 als geplante Plattformbibliothek festgelegt. In dieser Umgebung gibt es weder SDL3- noch SDL2-Entwicklungsheader/pkg-config-Dateien; nur die SDL2-Laufzeit ist vorhanden. Die verfuegbaren Paketquellen bieten `libsdl2-dev`, aber kein `libsdl3-dev`. Um die freigegebene SDL3-Architektur nicht stillschweigend zu wechseln, wird vorerst der testbare Software-/Headless-Backend verwendet. Ein natives Fenster, OS-Ereignispolling, Hardwarebeschleunigung, Audio und VSync sind damit **nicht** implementiert.
