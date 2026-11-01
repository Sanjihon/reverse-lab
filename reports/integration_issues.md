# Phase 17: Integrationsprobleme und offene Punkte

1. **SDL3 fehlt:** Die Phase-12-Entscheidung sieht SDL3 als Plattformgrenze vor, aber SDL3-Entwicklungsheader sind nicht installiert/verfuegbar. Der SoftwareRenderer/virtuelle WindowManager und Null-/WAV-Audioausgang sind integriert; ein echtes Fenster, SDL-Eventpolling, Audio-Device und Hardware-VSync fehlen.
2. **Kein Startprogramm:** `application_core` ist eine Library. Es existiert noch kein `main()`/GUI-Executable oder Install-/Paketierungsziel.
3. **Inputgeraete:** `InputManager` akzeptiert normalisierte Events. Kein OS/SDL3-Adapter enumeriert reale Geraete; mehr als zwei Controller werden ignoriert. Rumble, Remapping-UI und persistente Profile fehlen.
4. **Combat neu entwickelt:** Hit-/Hurtbox-Masse, Attackreichweite, Schaden, Cooldown und Angriffsprioritaet sind Prototypewerte. Wenn beide Spieler im selben Tick treffen koennten, verarbeitet `CombatSystem` P1 zuerst und liefert maximal einen Treffer. Das ist keine originale SF2-Regel.
5. **Combat unvollstaendig:** kein Guard, Hitstun, Knockback, Recovery, Unverwundbarkeit, Spezialmove-Dispatch oder echte Attack-/Trefferanimation. Der Inputbuffer-/Special-Move-Recognizer dispatcht noch keine Combataktionen.
6. **Animation/Assets:** grafische Inhalte sind Laufzeitcheckerboards. Attack-, Victory-, Defeat- und Hitclips haben keine Character-Assets; nur Movement-State und generischer Hit-Reaction-Pfad sind verbunden.
7. **Fehlerbehandlung:** Initialisierungs-/Framefehler und Speicherallokationsfehler werden in `GameApplication` als Status/Errortext gemeldet. Unbekannte oder dritte Controllerereignisse werden vom InputManager ignoriert statt in ein Diagnoseprotokoll geschrieben.
8. **Originalbefunde:** keine bisherige Rekonstruktion liefert ein verifiziertes Spieler-/Combat-/Hitboxlayout oder vollstaendige SF2-Regeln. `src/` und ROM-/Ghidra-Dateien wurden weder eingebunden noch veraendert.

Diese Arbeit integriert ausschliesslich Phase 12-17. Live-Plattformadapter und Gameplay-Praezisierung benoetigen SDL3-Entwicklungsabhaengigkeiten und weitere Freigaben.
