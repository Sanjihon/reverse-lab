# Phase 12: Architektur der eigenstaendigen Implementierung

## Leitlinien

1. `implementation/` ist ein neues Programm und keine Fortsetzung oder ABI-Portierung des 68000-Codes.
2. Der deterministische Simulationskern kennt weder SDL-Typen noch Hardwareadressen, Ghidra-Namen oder ROM-Layouts.
3. Unbekannte Originalsemantik wird nicht als verifiziertes Spielverhalten uebernommen. Neue Regeln erhalten eigene, testbare Definitionen.
4. Grafik, Audio, Eingabe, Controller und Betriebssystemressourcen sind Adapter am Rand.
5. Original-ROMs, `working/maincpu.bin` und Ghidra-Projekt bleiben unangetastet.

## Laufzeitfluss

```text
SDL3 events / controller
          |
          v
   Input Manager ----> InputSnapshot[player]
                            |
                            v
Game Loop -> Game Engine -> Game State Manager
                            |
        +-------------------+------------------+
        |                   |                  |
        v                   v                  v
 Player Manager      Movement/System     Combat/Collision
        |                   |                  |
        +-------------------+------------------+
                            v
                 Character/Animation
                    |             |
                    v             v
             RenderSnapshot   AudioEvent queue
                    |             |
                    v             v
             SDL3 Renderer   SDL3 Audio adapter
```

Die Core-Seite verarbeitet eine feste Simulationsschrittweite und immutable Input-Snapshots; Renderer und Audiogeraet konsumieren Ergebnisse. SDL-Ereignisse werden nicht direkt aus den Fighter-/Combatregeln abgefragt.

## Komponenten und Schnittstellen

| Komponente | Verantwortlichkeit | Geplante Grenze |
|---|---|---|
| Game Engine | Lebenszyklus, Verbindungen und geordnete Updatephase | `initialize()`, `tick(InputFrame)`, `render(RenderSnapshot)`, `shutdown()`; keine Regeln fuer einen einzelnen Zug. |
| Game Loop | Zeitakkumulation, feste Simulationsschritte, Renderkadenz und Pause | Liefert diskrete `SimTick`-Werte; begrenzt Catch-up-Schritte nach Fensterstillstand. Tickrate ist konfigurierbar und noch kein Originalverhaltensclaim. |
| Game State Manager | Explizite Zustandswechsel und Eintritt/Austritt | State-Interface oder `std::variant`; Zustandswechsel werden am Tickrand verarbeitet. Menue-/Kampf-/Pause-/Ergebniszustaende sind neue Produktregeln. |
| Player Manager | Zuordnung von zwei lokalen Spielern zu Figuren und Commands | Zwei Spieler-Slots mit `PlayerId`, `InputSnapshot` und `PlayerState`; kein MAME-Portbit im Simulationskern. |
| Input Manager | Keyboard-/Gamepad-Ereignisse auf logische Aktionen abbilden | SDL-Adapter erzeugt Aktionen wie Richtung, Punch, Kick, Start; Remapping und Controller-Hotplug bleiben hier. |
| Movement System | Neue Bewegungsregeln pro Tick anwenden | Reine Funktion ueber `MovementInput`, `MovementConfig` und `PlayerState`; Grenzwerte und Einheiten explizit testen. |
| Character System | Character-Definitionen und Konfiguration bereitstellen | Datengetriebene, validierbare Definitionen; keine Original-Fighter- oder Move-Daten ohne Quellen-/Rechtepruefung. |
| Animation System | Animation-State und Frameauswahl | Simulationsseitige Frame-ID/Phase; Assethandles werden erst im Renderer aufgeloest. |
| Collision System | Geometrische Kandidaten und Kollisionsresultate | Reine AABB-/Shape-Abfragen mit expliziter Einheitenkonvention; keine SDL- oder ROM-Abhaengigkeit. |
| Combat System | Angriff, Treffer, Guard und Kampfereignisse | Verarbeitet explizit modellierte Attack-/Hurt-Daten und Statebedingungen; Regeln sind neu, solange Originalbelege fehlen. |
| Audio Manager | Soundevents aus Game-/Combatereignissen dispatchen | Queue aus typisierten `AudioEvent`s; SDL-Device-/Stream-Lifetime im Audioadapter. |
| Graphics Renderer | Fenster, Renderziele, Texturen und Zeichnung | SDL3-Implementierung konsumiert einen plattformneutralen `RenderSnapshot`; keine Spielregeln im Renderer. |

## Gemeinsame Datenvertraege

Die spaetere Implementierung soll kleine, typisierte Datenobjekte verwenden:

- `InputSnapshot`: logische Aktionen fuer einen Simulationsschritt.
- `PlayerState`: Position, Geschwindigkeit, Blickrichtung und expliziter neuer State.
- `CharacterDefinition`: validierte Parameter, Animation- und Move-Referenzen.
- `CollisionResult`: beteiligte IDs, Achse/Normal und Kontaktstatus.
- `AudioEvent`: Sound-ID und optionale Lautstaerke/Pan-Parameter.
- `RenderSnapshot`: geordnete Sprites/Quads und UI-Elemente ohne SDL-Pointer.

Diese Typen sind Architekturvorschlaege, keine Behauptungen ueber originale ROM-Strukturen. Oeffentliche Schnittstellen gehoeren nach `implementation/include/`; private Implementierungen in das passende `implementation/src/<module>/`.

## Updateordnung und Determinismus

Vorgeschlagene Simulationsreihenfolge pro Tick: Eingaben einfrieren, Befehle/States aktualisieren, Bewegung berechnen, Kollisionspaare ermitteln, Combatregeln anwenden, Animation fortschalten, Events sammeln und RenderSnapshot publizieren. Die exakte Reihenfolge ist eine neue Architekturentscheidung und muss bei spaeteren Spielregeln versioniert und getestet werden. Rendering und Audiowiedergabe duerfen die Simulation nicht veraendern.
