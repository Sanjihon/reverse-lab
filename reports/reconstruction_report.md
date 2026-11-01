# Rekonstruktion des 68000-Programmabbilds

Stand: 2026-10-31. Set: MAME `sf2eb` (Street Fighter II: The World Warrior, World 910214). Grundlage sind ausschließlich die in Phase 1 per CRC identifizierten lokalen Programm-ROMs und die in Phase 2 belegten `ROM_LOAD16_BYTE`-Offsets.

## Ergebnis

Erstellt wurde [`working/maincpu.bin`](../working/maincpu.bin), ein 1.048.576 Byte (`0x100000`) großes 68000-Programmabbild. Es enthält die vier MAME-definierten Even/Odd-Paare in den Bereichen `0x000000-0x03ffff`, `0x040000-0x07ffff`, `0x080000-0x0bffff` und `0x0c0000-0x0fffff`.

| Kennung | Wert |
|---|---|
| Größe | 1.048.576 Byte (`0x100000`) |
| SHA-256 | `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd` |
| CRC32 | `94d86e85` |
| Belegung | 1.048.576 von 1.048.576 Adressen |
| Lücken / Überschneidungen | keine / keine |

Die SHA-256- und CRC32-Werte beziehen sich auf das rekonstruierte Abbild, nicht auf einzelne Chips.

## ROM-Prüfung und Adressierung

Jede Quelldatei ist `0x20000` (131.072) Byte groß. Die erwarteten MAME-CRC32-Werte wurden vor dem Aufbau erneut geprüft. „Erstes/letztes Byte“ bezeichnet die jeweiligen Quelldatei-Bytes; die Adressspanne enthält nur die durch MAME festgelegten Lane-Adressen.

| Paar | Lane | Quelldatei | MAME-Offset | Adressspanne | CRC32 | Erstes Byte | Letztes Byte |
|---|---|---|---:|---|---|---:|---:|
| P1 | Even, D15-D8 | `sf2e_30b.rom` | `0x00000` | `0x00000-0x3fffe` | `57bd7051` | `00` | `00` |
| P1 | Odd, D7-D0 | `sf2e_37b.rom` | `0x00001` | `0x00001-0x3ffff` | `62691cdd` | `00` | `00` |
| P2 | Even, D15-D8 | `sf2e_31b.rom` | `0x40000` | `0x40000-0x7fffe` | `a673143d` | `00` | `ff` |
| P2 | Odd, D7-D0 | `sf2e_38b.rom` | `0x40001` | `0x40001-0x7ffff` | `4c2ccef7` | `00` | `e8` |
| P3 | Even, D15-D8 | `sf2_28b.rom` | `0x80000` | `0x80000-0xbfffe` | `4009955e` | `ff` | `ff` |
| P3 | Odd, D7-D0 | `sf2_35b.rom` | `0x80001` | `0x80001-0xbffff` | `8c1f3994` | `d0` | `ff` |
| P4 | Even, D15-D8 | `sf2_29b.rom` | `0xc0000` | `0xc0000-0xffffe` | `bb4af315` | `0e` | `00` |
| P4 | Odd, D7-D0 | `sf2_36b.rom` | `0xc0001` | `0xc0001-0xfffff` | `c02a13eb` | `ee` | `00` |

Bei Big-Endian-68000-Wortzugriffen liefert die gerade Adresse das höherwertige Byte (D15-D8), die ungerade Adresse das niederwertige Byte (D7-D0). Die Bytes wurden entsprechend den MAME-Offsets alternierend in die Zieldatei geschrieben; es wurde keine zusätzliche Wort- oder Byte-Vertauschung angewendet.

### Randbytes der rekonstruierten Paarbereiche

| Paarbereich | Erstes Abbildbyte | Letztes Abbildbyte |
|---|---:|---:|
| `0x000000-0x03ffff` | `00` | `00` |
| `0x040000-0x07ffff` | `00` | `e8` |
| `0x080000-0x0bffff` | `ff` | `ff` |
| `0x0c0000-0x0fffff` | `0e` | `00` |

## Reproduzierbarer Build

Das Skript [`tools/rebuild_maincpu.py`](../tools/rebuild_maincpu.py) verwendet nur die Python-Standardbibliothek und die festgehaltenen MAME-Dateinamen, Offsets, Größen und CRCs. Es öffnet Original-ROMs mit `rb`, validiert Größe, CRC, Lane-Parität und Adressgrenzen, weist Überlappungen zurück und verlangt vollständige Adressabdeckung. Nach dem Aufbau vergleicht es jede extrahierte Even-/Odd-Lane byteweise mit der jeweiligen Quelle. Die Ausgabe wird zunächst als temporäre Datei unter `working/` geschrieben, geprüft und anschließend atomar als `working/maincpu.bin` veröffentlicht.

Reproduktion aus dem Projektverzeichnis:

```sh
python3 tools/rebuild_maincpu.py
```

Ergebnis des erfolgreichen Laufs:

```text
Output: working/maincpu.bin
Size: 1048576 bytes (0x100000)
SHA-256: 6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd
CRC32: 94d86e85
Coverage: 1048576/1048576 byte addresses; no gaps or overlaps
```

Es wurden keine Original-ROMs verändert. Es wurde kein Ghidra-Import oder anderer Programmlauf gestartet.