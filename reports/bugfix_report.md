# Phase 19: Fehleranalyse und Korrekturen

## Fehler 1: Eingabepuffer ueberlebte Controller-Neuzuordnung

- **Urspruengliches Problem:** Eine gedrueckte Attackaktion blieb im 5-Tick-Inputbuffer, nachdem der Controller von P1 zu P2 verschoben oder manuell abgemeldet wurde. Die Aktion konnte noch an den alten Spieler ausgegeben werden.
- **Ursache:** `assign_controller` und `unassign_controller` aenderten den Device-Handle, loeschten aber weder `previous_held` noch `buffered` und Motion-Historie.
- **Betroffene Dateien:** `implementation/src/input/input_manager.cpp`, `implementation/tests/input_tests.cpp`.
- **Aenderung:** Bei Zuordnungswechsel, Abmeldung und Disconnect werden alte Press-Latches, Buffer, Richtungsverlauf und relative Richtung geloescht. Beim Zuweisen wird der neue gehaltene Zustand ohne synthetischen neuen Press uebernommen.
- **Tests:** Neuer Regressionstest `controller reassignment clears stale buffer`; Input-Testziel wiederholt.
- **Ergebnis:** Reproduzierender Test schlug vor der Korrektur bei der gepufferten Light-Punch-Ausgabe fehl; nach der Korrektur bestehen alle 11 Inputtests.

## Fehler 2: App-Lifecycle bewahrte alte Simulations-/Geraetezustände

- **Urspruengliches Problem:** Shutdown/Reinitialize setzte GameSession und Ressourcen zurueck, liess aber GameLoop-Akkumulator, Simulationstakt, Controller-/Control-Latches und Combat-Cooldowns bestehen. Eine erneute Initialisierung konnte mit alter Eingabe oder Zeitbasis fortsetzen.
- **Ursache:** `GameApplication::shutdown()` und `initialize()` setzten nur Teile des Laufzeitzustands zurueck.
- **Betroffene Dateien:** `implementation/src/core/game_application.cpp`, `implementation/src/combat/combat_system.cpp`, `implementation/include/combat_system.hpp`, `implementation/src/input/input_manager.cpp`, `implementation/include/input_manager.hpp`, `implementation/tests/integration_tests.cpp`.
- **Aenderung:** `InputManager::clear_runtime_state()` entfernt Devices, Controls, Buffer, Press-Latches und History; App-Neustart/Shutdown setzt GameLoop, Ticknummer und Combat-Cooldowns zurueck. Rundenstart/Title-Wechsel setzt ebenfalls Attack-Cooldowns zurueck.
- **Tests:** `application shutdown and reinitialization` prueft Controllerloeschung, Title-/Playerreset; alle Integrationstests erneut ausgefuehrt.
- **Ergebnis:** Tests bestehen in Debug, Release und ASan/UBSan.

## Weitere Befunde

- Phase 18 hatte keine weiteren offenen reproduzierbaren Speicher-/Crashbugs dokumentiert.
- Blocken, Knockback, Trade-Semantik und SDL-/Hardwarepfade bleiben fehlende Funktionen, keine in Phase 19 behobenen Bugs.
- Es wurden keine Dateien unter `src/` oder der Ghidra-Rekonstruktion geaendert.
