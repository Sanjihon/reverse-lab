# Phase 13: Spielzustandsverwaltung

## Zustandsuebergaenge

| Zustand | Eingabe/Ereignis | Folgezustand |
|---|---|---|
| `Title` | globale Bestaetigung | `CharacterSelect` |
| `CharacterSelect` | beide Spieler bestaetigen | `FightIntro` |
| `CharacterSelect` | `cycle_character` vor Bestaetigung | waehlt das andere Trainingsprofil |
| `FightIntro` | globale Bestaetigung | `Fighting` |
| `Fighting` | Pause | `Paused` |
| `Paused` | Pause oder Bestaetigung | vorheriger Zustand, derzeit `Fighting` |
| `Fighting` | Spielerleben erreicht 0 via `apply_damage` | `RoundResult`; Rundensieg wird gezaehlt |
| `RoundResult` | Bestaetigung, noch kein Gesamtsieg | neue Runde mit vollem Leben, dann `FightIntro` |
| `RoundResult` | Bestaetigung, zwei Rundensiege erreicht | `Victory` fuer P1 oder `Defeat` fuer P1 |
| `Victory` / `Defeat` | Bestaetigung | `GameOver` |
| `GameOver` | Bestaetigung | Session wird nach `Title` zurueckgesetzt |
| jeder Zustand | `restart` | Session wird sofort nach `Title` zurueckgesetzt |

## Spielregeln und Abgrenzung

- Die Serie ist neu als Best-of-three mit zwei Siegen definiert.
- `apply_damage` akzeptiert nur positiven Schaden waehrend `Fighting`; keine Attacke erzeugt den Schaden automatisch.
- Pause friert Frame-/Bewegungsupdates ein.
- Game State sind Logikzustand, nicht gerenderte Screens. SDL3-Renderer und UI fehlen.
- Diese Zustaende sind eigenstaendige Implementierung, keine aus dem unbekannten Original-State-Machine-Code rekonstruierten Namen.
