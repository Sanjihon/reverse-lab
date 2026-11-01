# Phase 16: Spezialbewegungserkennung

## Musterdefinition

`SpecialMovePattern` ist eine Folge zeitgestempelter `MoveStep`s. Ein Schritt kann eine relative Richtung, null oder mehr gleichzeitig zu drueckende Buttons oder beides verlangen. Das Muster definiert `max_gap_ticks` zwischen passenden Schritten und ein Gesamtfenster `total_window_ticks`.

Richtungen werden anhand des aktuellen `PlayerState::facing` normalisiert: Forward/Back sowie Up/DownForward/DownBack sind relativ zum Spieler. Richtungswechsel, diagonale Uebergaenge und Button-Presses werden pro Frame in einem begrenzten Verlauf erfasst. Wiederholte identische Frames verbrauchen keine Sequenzschritte.

Beispiel Quarter-Circle-Forward + Light Punch:

```cpp
SpecialMovePattern{
    "quarter-circle-forward",
    {
        {RelativeDirection::Down, {}},
        {RelativeDirection::DownForward, {}},
        {RelativeDirection::Forward, {InputAction::LightPunch}}
    },
    6,
    12
}
```

Das Format unterstuetzt ebenso Half-Circle, Back-/Forwardsequenzen, reine Richtungswechsel und mehrbuttonige letzte Schritte. `recognizes` wertet die zuletzt aufgezeichneten Samples aus; Combataktionen werden dadurch noch nicht automatisch ausgefuehrt.

## Neue Regeln und Grenzen

Bufferdauer default 5 Simulationsticks, Special-Move-Gap default 12 Ticks und Gesamtfenster 30 Ticks sind neue Projektentscheidungen. Die Tests konfigurieren engere Fenster fuer reproduzierbare Beispiele. Ghidra/MAME belegen keine SF2-spezifischen Commandfenster oder Move-Kommandos; keine dieser Zeitangaben ist als Originalwert zu verstehen.
