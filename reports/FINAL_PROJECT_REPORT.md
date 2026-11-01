# Final Project Report

Stand: 2026-11-01. Das Projekt ist am Ende von Phase 20 dokumentiert. **Es wurde kein originalgetreues, vollstaendiges Street Fighter II-Spiel erstellt.**

## Gesamtbewertung

| Bereich | Bewertung | Beleg / Grenze |
|---|---|---|
| ROM-Identifikation und Speicherabbild | Technisch abgeschlossen | MAME-Set `sf2eb`, 23 lokale ROMs, 8 fehlende PLDs, `maincpu.bin` 1 MiB. |
| Ghidra-Analyse | Abgeschlossener statischer Projektstand | Ghidra 12.1.4, 353 Funktionsstarts/11.356 ReferenceManager-Xrefs; PCode-/Grenzwarnungen und indirekte Ziele bleiben. |
| Original-C-Rekonstruktion | Teilweise abgeschlossen | 1 bestaetigter partieller Reset-Prefix, 1 wahrscheinliches Overlap-Hostmodell fuer 2 Ghidra-Starts; 350 Starts ohne C-Implementierung. |
| Neue Softwarearchitektur und Core | Technisch gebaut, teilweise abgeschlossen | C++20-Core mit Session, Bewegung, Eingabe, Collision-/Combat-Prototyp, Grafik/Animation, PCM-Audio und Application-Orchestrierung. |
| Plattformspiel | Nicht abgeschlossen | Kein `main()`-Programm, kein natives SDL3-Fenster, kein Live-Audiodevice und keine Endnutzer-Anwendung. |
| Tests | Automatisiert bestanden, scope-begrenzt | 49 headless Testfaelle; kein Original-68000-Lauf und kein Hardware-/SDL-End-to-End-Test. |

Die Statuszeilen fuer alle Phasen 1-20 stehen in `reports/PROJECT_STATUS.md`. Inhaltliche Abschlussberichte: Rekonstruktion `FINAL_RECONSTRUCTION_REPORT.md`, neue Implementierung `FINAL_IMPLEMENTATION_REPORT.md`, Tests `FINAL_TEST_REPORT.md`, Architektur `PROJECT_ARCHITECTURE.md`, Build `BUILD_AND_INSTALLATION.md`, Grenzen `KNOWN_LIMITATIONS.md`, Restarbeiten `REMAINING_WORK.md`.

## Integritaet

- MAME `sf2eb` (World 910214): 23/23 vorhandene ROM-SHA-256-Werte stimmen mit `reports/rom_manifest.csv` ueberein; 8 PLDs fehlen laut MAME.
- `working/maincpu.bin`: 1.048.576 Bytes, SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd` (Sollwert stimmt).
- Ghidra-Datenbankhashes blieben bei den Analyse-/Exportlaeufen bytegleich. Keine Original-ROMs, das Image oder die Rekonstruktionsdateien wurden in Phase 20 geschrieben.
- `implementation/assets/` ist leer; Originalgrafik/-audio wurde nicht kopiert.

## Abschliessende Bewertung

- **Technisch abgeschlossen:** ROM-Identifikation und Speicherzuordnung, 1-MiB-Abbild, gespeicherter Ghidra-Analyse-/Katalogstand, getrennte Headless-CMake-Builds sowie Debug-/Release-/Sanitizer-Regressionslaeufe.
- **Teilweise abgeschlossen:** Source-Rekonstruktion (1 partieller CONFIRMED Prefix und 2 PROBABLE Funktionsstarts) und neue Softwareimplementierung (integrierter, testbarer Headless-Prototyp mit neuen Regeln).
- **Nicht abgeschlossen:** vollstaendige SF2-Quellrekonstruktion, originales oder originalgetreues spielbares Produkt, natives SDL3-Frontend und Verifikation des Laufzeitverhaltens gegen den 68000-Code.

## Historische und verbleibende Punkte

Die Resume-Archive vom 2026-11-01 sind Vor-Phase-10-Snapshots und enthalten nicht den heutigen Endstand. Verifizierte ROM-Information ist kein Nachweis fuer das Dekodieren aller ROM-Daten. Die Sound-ROMs wurden beispielsweise identifiziert, aber nicht dekodiert.

Alle Phase-20-Berichte trennen Originalbelege, rekonstruierte Teilmodelle, neue Prototypregeln und unbekannte Verhaltensbereiche. Weitere Entwicklung benoetigt eine neue ausdrueckliche Freigabe.
