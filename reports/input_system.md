# Phase 16: Eingabesystem

## Architektur

`InputManager` verarbeitet plattformneutrale `InputEvent`s aus einer spaeteren SDL3-/Betriebssystemadapter-Schicht. Er liest weder direkt SDL-Events noch CPS-MMIO. Ereignisse umfassen DeviceConnected/Disconnected, Keyboard-Tasten, Gamepad-/Joystickbuttons und analoge Achsen.

Pro Spieler gibt es ein separates Profil mit eigener Binding-Liste, Sensitivity, Deadzone, Bufferfenster und optional zugewiesenem Controller. `sample_frame(tick, session)` kombiniert gehaltene Bewegungen mit gepufferten Button-Presses zu `FrameInput`. `dispatch_frame` reicht dieses Paket an `GameSession::process_frame` weiter; dadurch erreichen Richtungen den bestehenden Movement System und Pause/Confirm die State Machine. Angriffsfelder werden im Inputpaket ausgegeben, aber es existiert noch kein Combat System, das sie verarbeitet.

## Tasten und logische Aktionen

Logische Richtungen sind Links/Rechts/Oben/Unten; P1-Up wird als Jump und Down als Crouch an die vorhandene Gameplay-Schnittstelle abgebildet. Sechs unabhängige Aktionen bilden Light/Medium/Heavy Punch und Kick ab. Lauf, Confirm, Charakterwechsel, Pause und Neustart sind ebenfalls belegbar.

Neue Defaulttasten: P1 Pfeile + J/K/L/U/I/O, Confirm `1`; P2 WASD + F/G/H/R/T/Y, Confirm `2`; gemeinsamer Laufmodifier Shift, Charakterwechsel C/V, Pause P und Neustart N. Das sind frei ersetzbare Vorschlaege, keine MAME- oder ROM-Belegung.

`bind` ergaenzt eine Zuordnung; `clear_bindings` setzt ein Spielerprofil zurueck; `set_profile_config` setzt Buffer-/Achsenwerte; `dispatch_frame` verbindet mit GameSession. Attack-Presses bleiben bis Verbrauch/Ablauf gepuffert; UI-Aktionen werden nach GameSession-Dispatch verbraucht.

## Zeitfenster und Originalbefunde

MAME belegt fuer `sf2eb` active-low P1/P2-Richtungen und drei Punch-/drei Kick-Buttons in den CPS-Ports. Die Ghidra-Analyse hat keine sichere ROM-Inputroutine oder absolut referenzierte Leseroutine nachgewiesen. Die neue Eingabelogik und Tastenbelegung ist deshalb nicht als Originalverhalten gekennzeichnet.

SDL3-Pollingadapter, Tastatur-/Gamepad-OS-Code und echte Hardwaretests fehlen, weil SDL3-Entwicklungsdateien in der Umgebung nicht installiert sind. Der normalisierte Eventeingang und alle Zuordnungs-/Frame-Regeln sind headless implementiert und getestet.
