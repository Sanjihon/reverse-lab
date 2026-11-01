# Remaining Work

Dieses Dokument priorisiert offene Fragen nach dem Phase-20-Endstand; es startet keine neue Phase.

## Originalprogramm-Rekonstruktion

1. Vollstaendigen Reset-/Initialisierungspfad und moegliche 68000-Code-/Datenüberlappungen um `0x426`, danach `0x7cc`, dynamisch oder mit gezielter Ghidra-Navigationspruefung validieren. Die bestehende Evidenz zeigt statisch `JMP (A4)` bei `0x750` nach `0x426`, dann Branch `0x42a -> 0x7cc`.
2. Indirekte Sprungziele bei weiteren `JMP (A4)` und berechneten Dispatches klaeren; Instruktionsgrenzen unter Beruecksichtigung der 488 PCode-Warnungen pruefen.
3. Sicheren Zugriffspfad zur MAME-IN1-/IN2-Inputhardware finden; MAME-Portdefinition ist kein Nachweis einer ROM-Leseroutine.
4. A5-relative Record-/Listenfelder und Kandidaten `FUN_0007d9b4`, `FUN_0007f1b4`, `FUN_00083588`, `FUN_00083838`, `FUN_000839ea` statisch/dynamisch korrelieren.
5. Sound-ROM `sf2_09.rom` und OKI-Sample-ROMs nur mit einem belegten, rechtlich und technisch geeigneten Decoder untersuchen. Nicht dekodierte Daten bleiben unbekannt.
6. Collision-Descriptoren, Angriff/Verteidigung, HP, Guard, Hitstun, Knockback und Character-/Move-Daten mit belastbaren ROM-/Runtime-Belegen verbinden.

## Neue Software

1. SDL3-Entwicklungsabhaengigkeit und native Window-/Event-/Gamepad-/Audio-Adapter fuer Zielplattformen bereitstellen oder SDL2-Wechsel gesondert freigeben.
2. `main()`-Executable, Paketierung und Cross-Platform CI hinzufuegen.
3. Menu/UI/Sprite-/Audioassets mit eigenen oder rechtmaessig lizenzierten Inhalten erstellen; keine ROM-Daten extrahieren, solange Rechte/Format nicht geklaert sind.
4. Combat-Prototyp um getestete Trades, Guard, Trefferreaktionen, Knockback und Animation-Events erweitern; neue Werte als neue Projektregeln kennzeichnen.
5. SDL-Hardwaretests, laengere Soaks, Rendering-/Audio-Liveprofile und Zielplattform-Performance messen.

Jede Fortsetzung benoetigt ausdrueckliche Freigabe. Dieses Dokument behauptet keine bereits ausgefuehrte Arbeit.
