# Kampfsystem: belegte und offene Pfade

## Belegte Kollisions-zu-Objektzustands-Kette

Der derzeit klarste combat-nahe Pfad ist ein generischer Objekt-/Colliderpfad:

1. `FUN_0007d6e2` (`0x7d6e2`) prueft Flags `A5+0xade` und `A5+0x2e3` und ruft `FUN_0007e3f6`/`FUN_0007e25e` auf.
2. `FUN_0007e25e` (`0x7e25e`) sammelt Achsen-/Parameterbits, vergleicht Objekt-Offsets und Extents ueber `FUN_0007e486`, waehlt ueber `DAT_0007e2f6` einen Handler.
3. Die Handler bei `0x7e30a`, `0x7e318`, `0x7e326`, `0x7e34c`, `0x7e35a`, `0x7e380` und `0x7e3a6` setzen A5-relative Ergebnisfelder.
4. Der Caller aktualisiert A2/A6-relative Status-/Objektfelder und ruft `FUN_0007d9b4` sowie `FUN_0007dcd6` auf.

Die direkte Call-Referenz von `0x7d702` auf `0x7e25e` und die internen Call-Sites der Helper sind im Ghidra-Inventar belegt. Die semantische Kette ist **Kollision -> Objektzustandsaktualisierung wahrscheinlich**; eine konkrete Zuordnung zu Kaempfer-vs-Kaempfer bleibt unsicher.

## Rendering ist nicht Trefferverarbeitung

`FUN_0007f1b4` (`0x7f1b4`) verarbeitet Descriptor-/Tile-Daten, clipped Koordinaten an `0x200` und emittiert Vier-Wort-Eintraege in einen Ausgabepuffer. Das ist als generische Sprite-/Objektlisten-Emission plausibel; es belegt weder Hitboxen noch Angriffsausfuehrung. `FUN_00009314`/`FUN_00009476` setzen GFX-/Layer-Daten auf und sind ebenfalls nicht als Schadens- oder Trefferhandler identifiziert.

## Trefferreaktion, Blocken und Knockback

Der Caller `FUN_0007d6e2` veraendert nach seinem kollisionsnahen Test mehrere A2/A6/A5-relative Statusfelder und ruft Folgefunktionen auf. Das koennte eine Kontakt-/Objektreaktion sein. Keines der untersuchten Felder wurde als HP, Schaden, Block, Trefferstun, Knockbackgeschwindigkeit oder Sprung-Lufttrefferzustand nachgewiesen.

`FUN_0007e486` verwendet Facing-/Orientierungsbytes bei `A2+0x12` und `A6+0x12`, um horizontale Box-Offsets zu spiegeln. Das ist Geometrie, kein Beweis fuer Blocken zur richtigen Seite.

## Unaufgeloeste Kampfpfade

- Keine sicher identifizierte Routine fuer Attack-Startup/Active/Recovery, Trefferflag, Guard, Hitstun, Knockback oder Schaden.
- Keine eindeutige P1/P2-Eingabeleseroutine im ROM-Referenzinventar; MAME-Portbitbelegung ist separat in `reports/player_control.md` dokumentiert.
- Kein belastbarer Link von UI-Strings wie `PLAYER SELECT`, `THE FIGHT IS ALL.` oder `LET'S START OVER.` zu Code: Ghidras Stringinventar findet 14 passende Stringobjekte und 0 eingehende Referenzen.
- Keine nachgewiesenen Runden-/Sieg-/Niederlagenbedingungen.
- Der grosse Kandidat `FUN_00002540` enthaelt Kontrollfluss-/Instruktionsueberlappungswarnungen und ist keine sichere Kampf-Hauptschleife.

Die einzige belastbare Interpretation ist daher ein generischer 2D-Overlap-Test mit zustandsabhaengigem Folgepfad. Weitere Kampfmechanik bleibt offen.