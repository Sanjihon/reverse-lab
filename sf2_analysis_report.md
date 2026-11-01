# SF2-ROM-Verzeichnis: statische Analyse

## Ergebnis in Kürze

- Untersucht wurden 23 Dateien in `/home/user/reverse-lab/sf2/`; es gibt keine Unterverzeichnisse. Gesamtgröße: 7.667.712 Byte (7,31 MiB). Die Originaldateien wurden nicht verändert und nicht ausgeführt.
- Die Dateien sind headerlose ROM-Dumps, keine erkennbaren ZIP-, ELF-, PE- oder Dateisystem-Images. Die Größen gruppieren sich in eine 64-KiB-Datei, zehn 128-KiB-Dateien und zwölf 512-KiB-Dateien.
- Die zwölf gleich großen `sf2gfx*.rom`-Dateien sind sehr wahrscheinlich Grafik-ROMs. `sf2_09.rom` sieht anhand des Anfangs `F3 ED 56 C3 6A 00` nach Z80-Programmcode aus und enthält die lesbare Zeichenfolge `version 4.25`.
- Die 128-KiB-ROMs scheinen mehrere Komponenten bzw. mögliche Revisionen zu enthalten. Eine verlässliche Chip-zu-Speicherkarte ließ sich aus den Dumps allein nicht belegen.
- Ghidra 12.1.4 wurde über REA tatsächlich verwendet. Der Import von `sf2e_30b.rom` als Big-Endian-68000-Raw-Binary bei Basis/Entry `0x0` ergab nur eine künstlich gesetzte Entry-Funktion mit sechs wiederholten `ori.b #0x4, Dn`-Instruktionen. Das ist kein belastbarer Nachweis echten Programmablaufs und spricht dafür, dass eine isolierte ROM-Lane oder ein falscher Chip importiert wurde.
- Der REA-Binwalk-Regionscan war nicht verfügbar: `REA_BINWALK_COMMAND` ist nicht konfiguriert. Deshalb sind eingebettete komprimierte Teilbereiche/Dateisysteme damit nicht systematisch ausgeschlossen. Die Entropie allein unterscheidet Kompression nicht von Verschlüsselung.

## Inventar

SHA-256 wurde für jede Datei vollständig berechnet. Die Entropiespalte ist die von REA bestimmte Shannon-Byte-Entropie über die gesamte Datei, in Bit/Byte. Sie ist kein Verschlüsselungsnachweis.

| Datei | Byte | Entropie | Vermutete Rolle | SHA-256 |
|---|---:|---:|---|---|
| `sf2_09.rom` | 65.536 | 6,9612 | Sound-CPU-Programm, vermutlich Z80 | `5cbe56f90d361694ebe8ccb71966665d1784497519d268f13f9c2e6284b42194` |
| `sf2_18.rom` | 131.072 | 7,3673 | Audio-/Sample-ROM oder andere Datenkomponente | `a79590915def8d3a6c9f1995573f0f0c251bbae42bb03bf3a7fb92cdd7227975` |
| `sf2_19.rom` | 131.072 | 7,2578 | Audio-/Sample-ROM oder andere Datenkomponente | `76e89bb1d939d2d3dd8470c33bfb388edc5761a9e311c82d95786b727f1a297d` |
| `sf2_28b.rom` | 131.072 | 4,1742 | Programm-/Board-ROM, Funktion offen | `f89051620a7ec1d6d618729a1bdbed9faa56836363ed783f847e42157542ef2b` |
| `sf2_29b.rom` | 131.072 | 3,6754 | Programm-/Board-ROM, Funktion offen | `a6200cdb9d73da6c3e043a49862b867b98f5c7505fffb9f8579c754148fc269a` |
| `sf2_35b.rom` | 131.072 | 4,9659 | Programm-/Board-ROM, Funktion offen | `801fb28d458c223b18aec4f4f811921678202e446db5a60576f7ffdfc98d7213` |
| `sf2_36b.rom` | 131.072 | 4,4569 | Programm-/Board-ROM, Funktion offen | `b7bceda5bd5656d65f21097caee1830498ade19a7a32e9df3ceacb3e8b37e7b9` |
| `sf2e_30b.rom` | 131.072 | 4,6485 | Programm-/Board-ROM, Funktion offen; Ghidra-Testdatei | `1451bd49711134342f474086798d3a6011244ad2892917a4d9800e3c1a290060` |
| `sf2e_31b.rom` | 131.072 | 4,6296 | Programm-/Board-ROM, Funktion offen | `6c2abfd4d23a20dd99b4d55f37c4718c4099f4bd4a0384eb9ecab30fc5c732d3` |
| `sf2e_37b.rom` | 131.072 | 6,3661 | Programm-/Board-ROM, Funktion offen | `6178537f4912bf890c94ceaf47f949008ea28ae7e6a336d0951ecd0c7dae0696` |
| `sf2e_38b.rom` | 131.072 | 5,9166 | Programm-/Board-ROM, Funktion offen | `2cab0ad27f8278de3e528baae72da8c2d7f8b43367deda02c00c35abd4a4f228` |
| `sf2gfx01.rom` | 524.288 | 5,1320 | Grafikdaten | `9eaa4e0c9d9efc11d6baa67b5d6fad5c628433dc7a6f19df0e02c329e17213fc` |
| `sf2gfx02.rom` | 524.288 | 5,1186 | Grafikdaten | `aedc4faba10475bfdea015ea5e24134488a4a9e40962ce021311e527223d87b4` |
| `sf2gfx03.rom` | 524.288 | 3,7895 | Grafikdaten | `864dda682ed964ec7643626af1eb429787a8f4e4e9f7e7f3dd41fe78e453f2be` |
| `sf2gfx04.rom` | 524.288 | 3,7948 | Grafikdaten | `5d652129d50000066617a1cf603fd27c90bb2b90c4f0b5e4a73bf4ca7fe1c5d7` |
| `sf2gfx10.rom` | 524.288 | 5,2784 | Grafikdaten | `d27787043e66a67036460f086a22fceed2da65396531092ec3d942f6f7b24b80` |
| `sf2gfx11.rom` | 524.288 | 5,2313 | Grafikdaten | `e613f40b734eb15213ee2f3482159ec54bce97f1d6129cabe48ceccca50d3045` |
| `sf2gfx12.rom` | 524.288 | 3,9925 | Grafikdaten | `dde8c85699d543d7c8811a575836ecca58d3d93ea4ef6290587268f83e03e206` |
| `sf2gfx13.rom` | 524.288 | 3,9613 | Grafikdaten | `220c55fc1840cb4a0b886846764e2622a4df94dcfe19c02e6941d56c2eb5505c` |
| `sf2gfx20.rom` | 524.288 | 5,5566 | Grafikdaten | `e28d8f365398b9099fe70d36f1b9181632b6ecd3d47d813b8c977fdc4c75c9a4` |
| `sf2gfx21.rom` | 524.288 | 5,6017 | Grafikdaten | `71150bc505e1b1b2249baf0e7c4cc7be988faf74f2a76e7e81d9d4f1b0186e81` |
| `sf2gfx22.rom` | 524.288 | 4,0147 | Grafikdaten | `6000a661130f6229d6a426d315fbc5568aea40bba56ceeb7caf232be3a0f0add` |
| `sf2gfx23.rom` | 524.288 | 4,0455 | Grafikdaten | `6814ea9c3a4e0708fe9e6b6c22f4549a8e065a6925974b7e3eed9b613fbded03` |

## Formate, Beziehungen und Architektur

- `file` klassifizierte fast alle Dateien nur als `data`. Zwei Ergebnisse sind wahrscheinlich zufällige Signaturtreffer in Rohdaten: `sf2gfx21.rom` wurde als OpenPGP Public Key und `sf2e_30b.rom` als Photoshop Color Swatch erkannt. Kein Beleg deutet darauf hin, dass diese Dateien tatsächlich solche Container sind.
- Kein ROM hat einen erkannten allgemeinen Dateisystem- oder ausführbaren Containerheader. Die Dateien verhalten sich wie rohe, feste Chip-Dumps. Keine Datei ist SHA-256-identisch mit einer anderen.
- Die 12 Grafik-ROMs bilden eine deutliche Größen-/Namensgruppe (`01..04`, `10..13`, `20..23`). Ihre Entropien von 3,7895 bis 5,6017 sind mit strukturierten Grafikdaten vereinbar und zeigen keine durchgehend hochentropische Codierung.
- Die vier `sf2_*b`- und vier `sf2e_*b`-Dateien könnten unterschiedliche Regionen/Revisionen oder parallel organisierte Programm-Bänke darstellen. Paarung, Byte-Lanes und Ladeoffsets sind aus Namen und Einzeldateien nicht sicher ableitbar.
- `sf2_09.rom` beginnt mit Bytes, die sich plausibel als Z80-Opcodes lesen lassen; die CPU-Familie Z80 ist daher eine begründete Vermutung. REAs Ghidra-Raw-Import akzeptierte hier nur `6502`, x86/x86-64, ARM/ARM64 und m68k, nicht Z80.
- Bei einem echten CPS-ähnlichen 68000-Programm wäre Big-Endian plausibel; der richtige Speicher- und Entry-Point ist für diese Chipdateien aber nicht bestätigt. Die im Ghidra-Test verwendeten Werte `m68k`, Big-Endian-68000-Standardlanguage und `0x0` waren explizite Analyseparameter, keine aus dem ROM-Header ausgelesenen Fakten.
- Entropieprofile: Die längsten als hoch klassifizierten Bereiche liegen in `sf2_18.rom` und `sf2_19.rom` (ab REA-Schwelle 7,2 Bit/Byte). Das passt zu Samples oder anderen dicht gepackten Daten, ist aber für sich allein kein Beweis für Kompression oder Verschlüsselung.

## Ghidra/REA-Befund

REA öffnete `sf2e_30b.rom` als headerloses Raw-Binary, Architektur m68k, Entry/Basis `0x0`; Ghidra identifizierte `68000:BE:32:default` und führte die Standardanalyse aus. Der binäre Hash blieb `1451bd49...1a290060`; die Analyse lief in einem ephemeren, schreibgeschützten Import.

Ghidra meldete eine einzige Funktion (`entry` bei `0x0`), 74 definierte Strings und einen einzigen Speicherblock von `0x0` bis exklusiv `0x20000`. Die Disassemblierung der ersten Bytes ergab wiederholte `ori.b #0x4`-Instruktionen. Es gab keine passenden `version`-, `copyright`-, `capcom`-, `street`-, `insert`-, `coin`- oder `game`-Strings im definierten String-Inventar. Diese Resultate sind wegen des nicht bestätigten Dateityps/Lane-Mappings nicht als Analyse einer vollständigen 68000-Firmware zu interpretieren; entsprechend gibt es keine belastbar identifizierten interessanten Funktionen, Referenzen oder Kontrollflussrouten.

## Offene Fragen und Grenzen

- Welche Dateien gehören genau zu einem gemeinsamen ROM-Satz, und welche sind alternative Revisionen oder Regionen?
- Welche 128-KiB-Dateien sind 68000-Programmchips, welche gehören zu anderen Komponenten, und wie sind eventuelle 16-Bit-Lanes sowie Banks zu kombinieren?
- Welche Ladeadressen, Resetvektoren und Speicherbereiche gelten auf der tatsächlichen Platine?
- Ist in den höherentropischen Daten eine proprietäre Kompression oder Verschlüsselung enthalten? Die vorliegenden Messungen entscheiden das nicht.
- Ein vollständiger Firmware-Regionscan war mangels konfiguriertem Binwalk nicht möglich. Z80-Disassembly war über den verfügbaren REA-Ghidra-Raw-Import nicht möglich.

Für eine belastbare Tiefenanalyse werden die Board-/MAME-Loaddefinitionen oder ein gesicherter ROM-Manifestdatensatz benötigt. Damit lassen sich die richtigen Revisionen und Lane-Paare zu einem abgeleiteten temporären Speicherabbild zusammensetzen und dieses anschließend passend in Ghidra analysieren, ohne die Original-ROMs anzufassen.
