# Phase 12: Implementierungs-Roadmap

Dies ist ein Plan, keine Implementierung. Phasen nach Phase 12 sind nicht begonnen und benoetigen die jeweilige Freigabe.

## Vorbedingungen

- SDL3-Entwicklungsheader und CMake-Paket in den gewaehlten Buildumgebungen bereitstellen; derzeit lokal nicht installiert.
- Zielplattformen, Fenster-/Inputanforderungen und Assetrechte bestaetigen.
- Build- und Testpolicy festlegen, einschliesslich Compilerwarnungen und Sanitizern.

## Vorgeschlagene Arbeitspakete

| Reihenfolge | Paket | Lieferumfang | Abnahmekriterien |
|---:|---|---|---|
| 1 | Core/Build-Skeleton | CMake Targets fuer Core, App und Tests; keine SDL-Abhaengigkeit im Core | Linux headless configure/build/test; keine Aenderung an `src/` oder Ghidra. |
| 2 | Zeit und Zustandsfluss | feste Simulationsticks, Game Engine, State Manager mit einfachen Teststates | Deterministische Tickfolgen, Pause und Statewechsel per Unit-Test. |
| 3 | Inputadapter | logische Actions, Keyboard-/Controller-Mapping ueber SDL3 | Core bekommt nur Snapshots; Controller connect/disconnect und Remapping testbar. |
| 4 | Renderer-Vertikale | SDL3-Fenster, Renderer und RenderSnapshot; noch kein Kampfcontent | Plattformadapter rendert Testgeometrie und verlaesst sich nicht auf ROM-Assets. |
| 5 | Zwei Spieler und Bewegung | Player Manager, neue Bewegungsparameter und Grenzen | Reproduzierbare Positions-/Geschwindigkeitstests fuer Grenzwerte. |
| 6 | Character-/Animationsschema | eigene Datenvalidierung, Animation-Playback und minimale Platzhalterdefinition | Ungueltige Definitionen abgelehnt; Timing und Framewechsel getestet. |
| 7 | Collision und Combat | neu spezifizierte Shape-/Attack-/Guard-Regeln | Grenz-/Prioritaets-/Statefaelle in headless Tests; Regeln ausdruecklich als neu entwickelt markiert. |
| 8 | Audioevents und Inhalte | SDL3-Audioadapter, lizenzierte oder eigene Assets | Audio kann aus, Events bleiben deterministisch; Rechte/Quelle dokumentiert. |
| 9 | Produktreife | Menues, Match-Flow, Controller-Zuordnung, Paketierung und Cross-Platform CI | Zielplattformmatrix und Regressionstests bestanden; keine Kopplung an ROM-Analyse. |

## Ausserhalb dieses Plans

Keine autonome Code-Rekonstruktion aus unbekannten ROM-Funktionen, kein ROM-Loading, kein Kopieren von Originalgrafik/-audio, keine Behauptung einer Arcade-Verhaltensgleichheit und kein vollstaendiger spielbarer Fighter in Phase 12.
