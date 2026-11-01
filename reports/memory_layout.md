# MAME-Speicherbelegung: `sf2eb`

## Grundlage

Die Werte stammen aus MAMEs `ROM_START(sf2eb)`-Definition in [`cps1.cpp`](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L7199), geprüft am 2026-10-31. `CODE_SIZE` ist dort als `0x400000` definiert. Alle Adressbereiche unten sind inklusive Endadresse; MAME-Offsets werden nicht aus Chipnamen abgeleitet.

`ROM_LOAD16_BYTE` platziert je Quelldatenbyte an jeder zweiten Zieladresse. Beim 68000 sind gerade Adressen die höherwertige Byte-Lane (D15-D8) und ungerade Adressen die niederwertige Byte-Lane (D7-D0). Die Paarung und alle acht belegten Byte-Adressfolgen sind in [rom_pair_mapping.csv](rom_pair_mapping.csv) dokumentiert.

## Regionsübersicht

| MAME-Region | Größe | Deklarierte Loads | Belegung aus der Definition |
|---|---:|---|---|
| `maincpu` | `0x400000` | 8 × `ROM_LOAD16_BYTE`, je `0x20000` Quelldatenbytes | `0x000000-0x0fffff` vollständig belegt; `0x100000-0x3fffff` ohne ROM-Load |
| `gfx` | `0x600000` | 12 × `ROM_LOAD64_WORD`, je `0x80000` Bytes | Drei lückenlos belegte Abschnitte: `0x000000-0x1fffff`, `0x200000-0x3fffff`, `0x400000-0x5fffff`; in jedem Abschnitt belegen vier 16-Bit-Loads die vier Wortpositionen eines 64-Bit-Transfers |
| `audiocpu` | `0x18000` | 1 × `ROM_LOAD` plus `ROM_CONTINUE`, je `0x8000` Bytes | `0x00000-0x07fff` und `0x10000-0x17fff`; `0x08000-0x0ffff` unbelegt |
| `oki` | `0x40000` | 2 × `ROM_LOAD`, je `0x20000` Bytes | `0x00000-0x1ffff` und `0x20000-0x3ffff`; vollständig belegt |
| `aboardplds` | `0x0200` | 5 × `ROM_LOAD`, je `0x0117` Bytes | Fünf deklarierte Einträge liegen sämtlich bei `0x0000-0x0116` und überlappen dort |
| `bboardplds` | `0x0200` | 2 × `ROM_LOAD`, je `0x0117` Bytes | Beide Einträge liegen bei `0x0000-0x0116` und überlappen dort |
| `cboardplds` | `0x0200` | 1 × `ROM_LOAD`, `0x0117` Bytes | `0x0000-0x0116`; `0x0117-0x01ff` ohne ROM-Load |

Die PLD-Regionen sind getrennte MAME-ROM-Regionen, keine CPU-Adressbereiche. Da MAME für mehrere PLD-Dateien identische Regionsoffsets angibt, wird hier die Überlappung dokumentiert und keine nicht belegte fortlaufende Anordnung erfunden. Die acht PLD-Dateien fehlen im lokalen ROM-Verzeichnis laut Phase-1-Manifest.

## 68000-Programmabbild

| Paar | MAME-Offsets (even / odd) | Quelldateigröße je Chip | Zusammenhängender Paarbereich |
|---|---|---:|---|
| P1 | `0x00000` / `0x00001` | `0x20000` (`131072`) | `0x000000-0x03ffff` |
| P2 | `0x40000` / `0x40001` | `0x20000` (`131072`) | `0x040000-0x07ffff` |
| P3 | `0x80000` / `0x80001` | `0x20000` (`131072`) | `0x080000-0x0bffff` |
| P4 | `0xc0000` / `0xc0001` | `0x20000` (`131072`) | `0x0c0000-0x0fffff` |

Jedes Quell-ROM enthält `0x20000` Bytes und belegt damit eine Byte-Lane über `0x40000` Byteadressen. Die beiden komplementären Lanes jedes Paars füllen gemeinsam genau den jeweiligen Paarbereich. Die vier Paarbereiche grenzen ohne Lücke oder Überlappung aneinander. Zusammen ergeben sie ein lückenloses, `0x100000` Byte großes 68000-Programmabbild von `0x000000` bis `0x0fffff`. Die MAME-Region ist `0x400000` groß; die darüber hinausgehenden `0x300000` Bytes haben in diesem Set keine Programm-ROM-Loads.

## LOAD-, CONTINUE- und RELOAD-Prüfung

- Programm-ROMs: acht `ROM_LOAD16_BYTE`-Anweisungen an den oben genannten Offsets. Keine `ROM_CONTINUE`- oder `ROM_RELOAD`-Anweisung im Programm-ROM-Bereich.
- Grafik-ROMs: zwölf `ROM_LOAD64_WORD`-Anweisungen an Offsets `0x000000/2/4/6`, `0x200000/2/4/6` und `0x400000/2/4/6`. Jede Quelldatei ist `0x80000` Bytes groß.
- Audio-CPU: `sf2_9.12a` lädt zunächst `0x8000` Bytes bei `0x00000`; `ROM_CONTINUE` lädt die verbleibenden `0x8000` Quelldatenbytes bei `0x10000`.
- OKI-Samples: zwei einfache `ROM_LOAD`-Anweisungen ohne Fortsetzung oder Wiederholung.
- PLDs: einfache `ROM_LOAD`-Anweisungen; die mehrfach identischen Startoffsets in den Board-PLD-Regionen sind explizite Überlagerungen.
- Im vollständigen `ROM_START(sf2eb)`-Block gibt es keine `ROM_RELOAD`-Anweisung.

## Abschlussprüfung

**Ja.** Die acht nach CRC zugeordneten Programm-ROMs ergeben gemäß MAMEs tatsächlichen Offsets und `ROM_LOAD16_BYTE`-Lanes ein zusammenhängendes 1-MiB-68000-Programmabbild. Die Big-Endian-Zuordnung ist: even = höherwertiges Byte, odd = niederwertiges Byte. Dies bestätigt die formale Speicherbelegung; es wurde kein zusammengeführtes ROM erzeugt und keine weitergehende Ausführung oder funktionale Programmanalyse vorgenommen.