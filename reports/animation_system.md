# Phase 14: Animationssystem

## Datenmodell und Playback

`AnimationClip` enthaelt einen Namen, Frames mit Quellrechteck und positiver Dauer in Simulationsticks sowie einen Loop-Flag. `AnimationController` unterstuetzt:

- Wechsel der Bewegung durch `sync_movement(MovementState, clips)`; bei Zustandswechsel startet der Clip am ersten Frame.
- Direkten Clipwechsel ueber `play_clip`, fuer spaetere Attack-/Hit-/Special-/Victory-/Defeat-Events.
- Per-Tick-Framefortschritt; Loop-Clips kehren nach dem letzten Frame zum Anfang zurueck.
- Nicht-loopende Clips bleiben nach Ablauf auf dem letzten Frame und melden `finished()`.
- Auslesen des aktuellen Frame-Rechtecks, Namens und Index.

Eine `.anim`-Definition verwendet zum Beispiel:

```text
loop false
frame 0 0 16 16 4
frame 16 0 16 16 6
```

Die Zeitbasis ist die 60-Hz-Simulationstaktung des neuen Kerns. Sie ist kein verifiziertes SF2-Frame-Timing.

## Verknuepfung und Grenzen

Die locomotion API nimmt `MovementState` aus dem Gameplay-Core entgegen. Ein Aufrufer ordnet MovementStates benannte Clips zu; der Controller enthaelt keine SDL-/Renderer-Abhaengigkeit. `play_clip` kann generische Kampf-/Siegclips starten, aber Phase 14 liefert weder Attack-/Hitstun-Aktionszustandsuebergaenge noch Originaldaten fuer diese Clips. Es gibt keine automatisch erstellten Character-Assets, keine Interpolation und keine Behauptung, dass ein Frameinhalt dem Original entspricht.
