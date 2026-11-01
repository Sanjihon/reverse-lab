# Phase 13: Spieler und Charakterzuordnung

## Datenmodell

`PlayerState` in `implementation/include/game_types.hpp` enthaelt:

- `PlayerId` fuer Spieler 1/2.
- `CharacterId` fuer zwei neutrale Trainingsprofile.
- Position und Geschwindigkeit als `Vec2`.
- `MovementState`, `ActionState` und `Facing`.
- Lebensenergie, Rundensiege, Auswahl-Lock und Jump-Pressed-Latch.

`PlayerManager` initialisiert Spieler 1 links mit Blick nach rechts und Spieler 2 rechts mit Blick nach links. Im Fight werden Blickrichtungen anhand der horizontalen Gegnerposition aktualisiert. Charakterauswahl kann pro Spieler zwischen `TrainingBalanced` und `TrainingHeavy` wechseln; beide Spieler muessen bestaetigen, bevor der Kampfvorbereitungszustand startet.

## Prototypwerte

| Neues Profil | Lebensenergie | Gehen | Laufen | Sprungimpuls | Gravitation/Tick |
|---|---:|---:|---:|---:|---:|
| `TrainingBalanced` | 1000 | 2.0 | 3.5 | -8.0 | 0.45 |
| `TrainingHeavy` | 1100 | 1.7 | 2.8 | -7.2 | 0.48 |

Alle Werte und Bezeichnungen sind **neu entwickelt** und dienen nur einer testbaren Basis. Keine dieser Zahlen stammt aus einer verifizierten SF2-Struktur oder Ghidra-Funktion.

Die Spielerpositionen starten bei X=160/480, Boden Y=300 und werden auf X=20..620 begrenzt. Das sind neue Weltkoordinaten, keine CPS-Board-Adressen.

## Grenzen

`ActionState` ist derzeit nur `Neutral`; Attack-, Guard- und Trefferzustandswechsel existieren nicht. Charaktere besitzen keine Originalnamen, Sprites, Moves, Audio-IDs oder ROM-Tabellen. Keine unbekannte A5-/A2-/A3-Struktur wird als verifiziertes Spielerlayout benutzt.
