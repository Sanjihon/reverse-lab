# SF2-ROM-Identifikation (Phase 1)

Stand: 2026-10-31. Untersuchungsumfang: `/home/user/reverse-lab/sf2/` und das zugehörige Archiv `/home/user/reverse-lab/sf2.zip`.

## Ergebnis

- Gefunden: **23 ROM-Dateien** in einem Verzeichnis, keine Unterverzeichnisse.
- Identifiziert: **23 von 23 vorhandenen Dateien**; 8 Programm-ROMs, 12 Grafik-ROMs und 3 Sound-ROMs.
- Exakte MAME-Revision: **`sf2eb` — Street Fighter II: The World Warrior (World 910214)**, B-Board 90629B-2.
- CRC32-Abweichungen gegenüber der MAME-Definition: **keine** bei den 23 vorhandenen ROMs.
- MAME-Definitionseinträge ohne entsprechende Datei im Verzeichnis: **8 PLD-Dateien**, jeweils 279 Byte laut MAME. Sie sind als fehlend markiert, nicht als CRC-Abweichungen.
- Gefundene Gesamtgröße: **7.667.712 Byte**. Erwartete Größe einschließlich der acht fehlenden PLDs: **7.669.944 Byte**.
- Größen der vorhandenen ROMs: 1 × 65.536 Byte, 10 × 131.072 Byte, 12 × 524.288 Byte.
- `unzip -t /home/user/reverse-lab/sf2.zip`: alle 23 Einträge bestanden die ZIP-CRC-Prüfung.

Die MAME-Setdefinition `sf2eb` ist in [MAME `cps1.cpp`, ROM-Definition](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L7198-L7245) dokumentiert. Die Registrierung benennt die Revision als „Street Fighter II: The World Warrior (World 910214)“ ([MAME game registration](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L15026)). Quelle geprüft am 2026-10-31.

## Klassifikation

- **Programm-ROMs (68000 main CPU):** die acht `sf2e_*`/`sf2_*b`-Dateien. CRC32 und jeweilige MAME-Chipbezeichnung stimmen in allen acht Fällen überein.
- **Grafik-ROMs:** die zwölf `sf2gfx*.rom`-Dateien. Sie wurden den MAME-Grafikchips über CRC32 zugeordnet; dadurch kann die vereinfachte lokale Dateibenennung unabhängig von der MAME-Chipbezeichnung identifiziert werden.
- **Sound:** `sf2_09.rom` entspricht dem MAME-Sound-CPU-ROM `sf2_9.12a`; `sf2_18.rom` und `sf2_19.rom` entsprechen den beiden MAME-OKI-Sample-ROMs.
- **Fehlend:** MAME führt in `sf2eb` zusätzlich fünf A-Board-, zwei B-Board- und ein C-Board-PLD-ROM. Keines dieser acht Objekte liegt im untersuchten Verzeichnis.

## Prüfmethoden und Grenzen

Dateigrößen stammen aus den Dateimetadaten. CRC32-IEEE wurde direkt aus den ROM-Bytes mit `zlib.crc32` berechnet; SHA-256 wurde direkt aus denselben Bytes mit `hashlib.sha256` berechnet. Die Quelldateien wurden nur gelesen. Die MAME-CRC32-Felder wurden mit den berechneten Werten verglichen; die Klassifikation erfolgte durch die MAME-ROM-Zuordnung (Region/Chipbezeichnung) sowie CRC32.

Binwalk v2.3.3 wurde ohne Extraktionsoption über alle vorhandenen ROMs ausgeführt. Es meldete an mehreren Offsets MySQL-/Boot-Header-Signaturen, die im Kontext der Roh-ROMs und der vollständig übereinstimmenden MAME-CRC-Werte als zufällige Signaturtreffer zu behandeln sind; es wurde kein bestätigtes zusätzliches Archiv oder Dateisystem identifiziert. Der REA-Binwalk-Provider war nicht konfiguriert (`REA_BINWALK_COMMAND` fehlt), daher lief dieser Signaturscan lokal. Es wurde weder extrahiert noch etwas ausgeführt.

Keine ROM-Datei wurde verändert oder zusammengeführt. Es wurden keine Speicheradressen aus den MAME-Loaddefinitionen in diesen Bericht übernommen; Speicherrekonstruktion ist nicht Teil dieser Phase.

## Fehlende MAME-Definitionseinträge

| MAME-Dateiname | Komponente | MAME-Größe | MAME-CRC32 | Status |
|---|---|---:|---|---|
| `buf1` | A-Board-PLD | 279 | `eb122de7` | fehlt |
| `ioa1` | A-Board-PLD | 279 | `59c7ee3b` | fehlt |
| `prg1` | A-Board-PLD | 279 | `f1129744` | fehlt |
| `rom1` | A-Board-PLD | 279 | `41dc73b9` | fehlt |
| `sou1` | A-Board-PLD | 279 | `84f4b2fe` | fehlt |
| `stf29.1a` | B-Board-PLD | 279 | `043309c5` | fehlt |
| `iob1.11d` | B-Board-PLD | 279 | `3abc0700` | fehlt |
| `c632.ic1` | C-Board-PLD | 279 | `0fbd9270` | fehlt |

Vollständige Bestands-, CRC- und SHA-256-Tabelle: [rom_manifest.csv](rom_manifest.csv). Kompakter CRC-Abgleich: [crc_report.txt](crc_report.txt).

**Phasenstatus:** Phase 1 abgeschlossen. Es wurde keine ROM zusammengeführt, keine Ladeadresse geraten und keine Speicherrekonstruktion begonnen. Warte auf Freigabe für Paket 2.
