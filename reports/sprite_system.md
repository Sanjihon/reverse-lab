# Phase 14: Sprite- und Ressourcensystem

## Sprite-Darstellung

`SpriteDrawCommand` enthaelt Texturhandle, Quellrechteck, Zielrechteck, RenderLayer, Sortierordnung und horizontalen Flip. Damit koennen mehrere Frames aus einem Spritesheet verwendet und in Background-/Stage-/Character-/Effects-/UI-Layer einsortiert werden. Der Software-Renderer zeichnet per Nearest-Neighbor-Sampling und Alpha-Blending in den logischen Framebuffer.

Character-, Kampf-, Treffer-, Sprung-, Spezial-, Sieg- und Niederlagenanimationen koennen als benannte Clips/Framebereiche beschrieben werden. Es gibt keine dazugehoerigen Originalsprites oder fertigen SF2-Animationen. `create_placeholder` erzeugt sichtbar einfarbige Checkerboard-Texturen zur Engine-/Rendererpruefung; sie werden nicht als Originalcontent ausgegeben.

## Ressourcenformat

- **Textur:** binaeres PPM P6 mit RGB-Kanaelen und Maxwert 255. Alpha ist fuer geladene PPM-Pixel deckend; programmatische Platzhalter koennen Alpha enthalten.
- **Animation:** einfacher `.anim`-Text mit `loop true|false` und `frame x y width height duration_ticks` pro Frame.
- **Spritesheet:** PPM-Textur plus Quellrechtecke im Draw-Command/Animationsframe.
- **Cache/Freigabe:** `ResourceManager` cached Texturen anhand des absoluten normalisierten Pfads, liefert Handles, bietet `unload_texture`, `unload_animation` und `clear`. Pixelpuffer besitzen Standard-C++-RAII-Lebensdauer.

Kein PNG/JPEG/BMP-Decoder, Audioformat, Asset-Streaming oder asynchrones Laden ist implementiert. Es wird keine externe Decoderbibliothek eingebunden. Die Assets bleiben von `sf2/`, `working/maincpu.bin` und Ghidra-Daten getrennt.
