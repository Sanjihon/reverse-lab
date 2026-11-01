# Phase 11: Offene Punkte

Phase 11 endet mit den folgenden nicht validierten Bereichen. Unbekannte Semantik bleibt UNKNOWN; es wurden keine Folgeschritte aus Phase 12 begonnen.

1. **Reset-Folgepfad:** Der statische Ablauf fuehrt vom `JMP (A4)` bei `0x750` nach `0x426`, wo eine zweite LEA beginnt, danach verzweigt `0x42a` nach `0x7cc`. Der weitere Initialisierungspfad ab `0x7cc` ist nicht in C rekonstruiert oder dynamisch validiert.
2. **68000-Runtime:** Kein Emulator/CPU-Interpreter und kein Boardlauf wurde fuer einen Befehls-fuer-Befehl-Vergleich verwendet. Condition-Code- und Registervertrag des Collision-Adapters bleiben unbelegt.
3. **Inputroutine:** Ghidra hat keine absoluten Xrefs in MAMEs IN1-/DIP-Fenster; berechnete/indirekte Portzugriffe sind nicht ausgeschlossen. Kein Input-C-Code und kein Inputtest.
4. **Character-/Move-/Animationsdaten:** grosse Kandidaten `FUN_0007d9b4` und `FUN_0007f1b4` sind nicht als Character-, Move- oder Animationimplementierung bestaetigt. Keine Feld-/Tabellenlayouts als gesichert darstellen.
5. **Collisionsemantik:** generischer 2D-Overlap mit Vorzeichen-/Mirrorpfad ist plausibel. Nicht bewiesen sind Attackbox/Hurtbox-Rolle, alle Descriptorlaengen, Kontaktprioritaet, Spielzustandswirkung und konkrete Callerparameter.
6. **Pointerlaengen und Alignment:** die C-Adapter erhalten keine Objekt-/Descriptorlaengen. Pointergueltigkeit und gerade 68000-Basisadressen sind Voraussetzungen, nicht validierte Laufzeitbedingungen.
7. **Datenstrukturen und globale Variablen:** A5-relative Felder und Register-basierte Zugriffskontexte haben weiterhin unbekannte fachliche Namen; keine verifizierten Player-/Character-/Move-/Game-State-Structs oder C-globalen Spielvariablen.
8. **Dynamische Tests:** keine der 353 Ghidra-Funktionen wurde ausgefuehrt. Die 30 Assertions pruefen nur sechs Host-Helper und ein PROBABLE-Hostmodell.

## Freigabegrenze

Diese Arbeit erfuellt ausschliesslich Phase 11. Die Phase-10-Basisdateien `working/maincpu.bin`, Original-ROMs und Ghidra-Projekt wurden nicht veraendert. Weitere Analyse, Emulatoraufbau, ABI-Rekonstruktion oder Spielimplementierung benoetigt einen neuen ausdruecklichen Auftrag.
