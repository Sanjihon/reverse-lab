# Known Limitations

## Rekonstruktion

- Von 353 Ghidra-Funktionsstarts: 1 partieller CONFIRMED Side-Effect-Prefix, 2 PROBABLE Collision-Modelstarts, 350 ohne C-Implementierung. Keine vollstaendige Originalfunktion ist als originalgetreuer C-Code wiederhergestellt.
- Es gibt keine dynamische M68000-/CPS1-Ausfuehrung und keine originale Spielverhaltensvalidierung. Ghidra meldete 488 nichtfatale PCode-Warnungen.
- Resetpfad ist nur statisch bis `0x426 -> 0x7cc` rueckfuehrbar; Hauptschleife und volle Initialisierung sind nicht nachgewiesen. `0x428` ist Extension-Word der zweiten LEA.
- Keine sichere Inputpollingroutine, Spieler-/Character-/Move-/Animation-/HP-Struktur, Hitbox-/Hurtbox-Klassifikation oder Schadensformel.
- Sound-ROMs sind identifiziert, aber nicht dekodiert. Grafik-ROMs wurden nicht als Appassets verwendet.

## Neue Anwendung

- C++20-Core ist headless. Kein `main()`-Executable oder Installationspaket.
- SDL3-Entwicklungsdateien fehlen. Kein natives Fenster, Live-Controllerpolling, Audiodevice, VSync oder Hardwarebeschleunigung.
- Nur generische Profile `TrainingBalanced` und `TrainingHeavy`; keine SF2-Roster-/Move-/Sprite-/Musikdaten.
- Collision und Combat sind einfache neue AABB-/Schadensprototypen. Kein Blocken, Knockback, Hitstun, Recovery, Invulnerability oder Special-Move-Dispatch. Simultane Treffer werden nicht als Trade aufgeloest; der erste P1/P2-Kandidat gewinnt.
- Testassets sind temporaere WAV/PPM-Dateien und prozedurale Platzhalter; `implementation/assets/` ist leer.
- Kein mehrstuendiger/langfristiger Test und keine reale Cross-Platform-Hardwarematrix. Die Peak-RSS-Messung gilt fuer den aktuellen Linux-Testprozess.

## Historische Sicherung

Die Archive `reverse-lab_backup_20261101.tar.gz` und `reverse-lab_handover_20261101.tar.gz` sind Vor-Phase-10-Snapshots. Sie enthalten nicht den Phase-20-Endstand.
