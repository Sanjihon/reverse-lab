# Phase 16: Controller- und Joystick-Unterstuetzung

## Adaptervertrag

`InputEvent` stellt einen plattformneutralen Vertrag fuer Keyboard, Gamepad und Joystick bereit. Ein spaeterer SDL3-Adapter uebersetzt OS-/SDL-Device-IDs, Buttons, Achsen und Hotplugereignisse in dieses Modell. Die aktuelle Phase installiert keine Bibliothek und pollt kein physisches Geraet.

- Bis zu zwei Gamepad-/Joystickgeraete koennen gleichzeitig verbunden sein.
- Das erste neu verbundene Geraet wird automatisch einem freien Spielerprofil zugewiesen; das zweite dem anderen. Weitere Controller werden ignoriert, bis ein Slot frei ist.
- `assign_controller` kann die Zuordnung explizit aendern; ein Geraet gehoert jeweils hoechstens einem Spieler. Disconnect entfernt den Controller, seine gehaltenen Werte und gepufferte Presses.
- Gamepad- und Joystickbuttons unterstuetzen beliebige normalisierte Button-IDs. Defaults bilden sechs Arcade-Aktionen auf South/East/West/North und beide Schultertasten ab; das Mapping ist frei konfigurierbar.
- Achsen X/Y werden mit Vorzeichen fuer links/rechts/oben/unten ausgewertet. Profilwerte begrenzen Sensitivity und Deadzone auf 0..1; Aktivierungsschwelle ist `max(deadzone, sensitivity)`.
- D-Pad-Buttons, sechs Actionbuttons, Confirm, Character-Cycle, Pause und Restart haben standardisierte adapterneutrale IDs.

## Einschränkung

Tests injizieren normalisierte Controllerereignisse, nicht reale SDL-/USB-Ereignisse. Discovery, OS-Deadzone-Kalibrierung, Controller-Remapping-UI, Vibration/Rumble und native Joysticks mit mehr als den zwei Spielerslots sind nicht implementiert.
