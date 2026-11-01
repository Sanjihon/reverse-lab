# Phase 18: Bekannte Fehler und fehlende Funktionalitaet

## Bekannte Modellgrenzen

1. Combat-Regeln sind Prototypwerte, keine SF2-Verhaltensrekonstruktion: AABB-Reichweite, einfache Schadenswerte, globaler Cooldown.
2. Blocken, Guard, Knockback, Hitstun, Invulnerabilitaet und Recovery fehlen.
3. Gleichzeitige Treffer werden nicht als Trade aufgeloest: `CombatSystem` liefert maximal einen Hit und prueft P1 zuerst.
4. Attackbuttons ausserhalb `Fighting` erzeugen keine Aktionen. Special-Move-Recognizer dispatcht nicht ins Combat.
5. Nur zwei generische Trainingsprofile; keine SF2-Charakter, Moves oder Originalanimationsdaten.
6. Kein SDL3-Frontend: keine native Fenster-/Input-/Audiodevice-Integration. Die Tests laufen ausschliesslich headless.
7. Mehr als zwei Controller werden ignoriert. Unbekannte Controllerereignisse erhalten keine persistente Diagnosemeldung.
8. Kein eigenstaendiges `main()`-Executable oder Paket fuer Endnutzer; CMake baut Libraries und Testziele.

## Fehlerbehandlung

Application-Initialisierung liefert Fehlertext bei ungueltiger Fenster-/Combatkonfiguration und fehlgeschlagener Audioinitialisierung. Framefehler und `std::bad_alloc` werden als fehlgeschlagener `RuntimeTickResult` zurueckgegeben. Software-Renderer-/Resourcefehler propagieren als Exception zum Tickfaenger. Das ersetzt keine native SDL-Device-/Hotplug-Recovery.

## Integritaet

Phase 18 hat nur `implementation/`, Testdateien, Buildkonfiguration und Phase-18-Berichte geaendert. Original-ROMs, `working/maincpu.bin` und Ghidra-Datenbank blieben unveraendert.
