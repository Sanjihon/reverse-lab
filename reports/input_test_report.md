# Phase 16: Eingabetests

## Build/Test

Ausgefuehrt:

```sh
cmake --build implementation/build --target input_tests --parallel 2
ctest --test-dir implementation/build -R input_tests --output-on-failure
implementation/build/input_tests
```

Ergebnis: Input-/Gameplay-Core und Testziel bauten warnungsfrei; CTest bestand **1/1 Input-Testziel**, **10/10 Testfaelle** bestanden.

## Testfaelle

1. Gleichzeitige Keyboardbewegung/-aktion fuer P1 und P2.
2. Sechs Punch-/Kick-Aktionen gleichzeitig sowie frei belegte Tasten.
3. Button-Puffer, Verbrauch und Ablauf.
4. Richtungsumkehr und Up/Down-Ausgabe.
5. Hotplug, automatische P1/P2-Zuweisung, Dritter-Controller-Limit, Achsensensitivitaet und Disconnect.
6. Manuelle Gamepad-/Joystick-Zuordnung und Controllerbuttonaktionen.
7. Quarter-Circle-Forward + Punch mit Ablauf nach dem Gesamtfenster.
8. Half-Circle-Back relativ zur Blickrichtung.
9. Mehrbutton-Chord am letzten Spezialbewegungsschritt.
10. Frame-Dispatch in die GameSession fuer Titel-/Charakterauswahl.

## Aussagegrenze

Alle Hardwaretests speisen normalisierte Events direkt ein. Reale SDL3-Ereignisse und Controller wurden nicht erfasst, da kein SDL3-Entwicklungspaket verfuegbar ist. Eine Combatreaktion auf Punch/Kick oder Spezialbewegung ist noch nicht vorhanden; getestet werden Mapping, Buffer und Recognizer, nicht ein Angriff im Spiel.
