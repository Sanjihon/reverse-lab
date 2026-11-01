# Phase 17: Systemintegration

## Zentrale Reihenfolge

`GameApplication::tick(elapsed, events)` ist die Integrationsfassade. Pro Aufruf:

1. Normalisierte Keyboard-/Gamepad-/Joystickereignisse an `InputManager` weitergeben.
2. `GameLoop::advance` bestimmt die Zahl fester 60-Hz-Simulationsschritte und meldet verworfene Catch-up-Zeit.
3. Pro Simulationsschritt InputSnapshot erstellen und an `GameSession` geben. Die Session verwaltet GameState, Charakterauswahl, PlayerManager, Bewegung und Runde.
4. `CombatSystem` prueft die sechs Angriffsfelder gegen die aktuelle Bewegung/Position aus den Spielern; `CollisionSystem` berechnet neue, einfache Attack-/Hurtboxen.
5. Getroffene Spieler erhalten ueber `GameSession::apply_damage` Lebensverlust und Rundenausgang.
6. `AnimationController`s synchronisieren MovementState, schreiten Frames weiter und koennen eine generische Hit-Reaktion abspielen.
7. Gameplay-Events werden einmal aus der Session geleert und an `AudioEngine` weitergereicht.
8. Der Audiomixer rendert einen festen PCM-Block; danach erstellt der Software-Renderer einen Frame und der virtuelle `WindowManager` skaliert/prasentiert ihn.

## Modulverbindungen

| Modul | Integriert ueber | Status |
|---|---|---|
| Game Loop | `GameApplication::tick` und `GameLoop::advance` | fester 60-Hz-Akkumulator; rendert einmal je outer tick. |
| Input Manager | normierte Eventspanne -> `FrameInput` | P1/P2 Bewegungs-/Attackfelder; Presses werden nach Frame konsumiert. |
| Player Manager | gehalten in `GameSession` | Spielerzustand/Position/HP/Rundensiege werden fuer Combat/Rendering gelesen. |
| Movement | `GameSession::process_frame` | Eingabe aktualisiert Spielerposition und MovementState. |
| Collision | `CollisionSystem` innerhalb `CombatSystem` | neue AABB-/Reichweitenregeln, keine rekonstruierten SF2-Hitboxen. |
| Combat | `CombatSystem::resolve` + `GameSession::apply_damage` | sechs Attackbuttons, einfacher Cooldown, ein Treffer pro Tick. |
| Animation | je Spieler `AnimationController` | MovementState-Clip und generische Hit-Reaktion mit Platzhalterframes. |
| Graphics | SpriteDrawCommands -> SoftwareRenderer -> WindowManager | headless RGBA-Fläche und virtuelle Skalierung, kein OS-Fenster. |
| Audio | Session-Events -> AudioEngine -> PCM Output | Null/WAV-Ausgabe, State-Musik und Effekt-Cues. |
| Game State Manager | `GameSession` | Title/Select/Intro/Fight/RoundResult/Victory/Defeat/GameOver/Paused. |

## Integrationsergebnis

`application_core` verbindet Gameplay, Grafik, Audio, Input, Collision und Combat ohne SDL-Abhaengigkeit. Die Anwendung wird als Bibliothek/Testziel gebaut; ein `main()`-Programm mit OS-Event-Polling, nativer Fensteranlage und Live-Audiodevice existiert noch nicht.

Angriffe, Hitboxen, Schadenswerte, Prioritaet und Cooldowns sind eigenstaendige, neue Prototype-Regeln. Die Phase-1-bis-11-Rekonstruktion bleibt getrennt und dient nicht als Behauptung, dass diese Regeln originale SF2-Mechaniken wiedergeben.
