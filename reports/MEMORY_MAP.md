# Street Fighter II: Speicherkarte

## Basis und Raw-Image

- Set: MAME `sf2eb`, World 910214.
- Prozessor: Motorola 68000, Big Endian, 16-Bit-Datenbus.
- Ghidra: `68000:BE:32:default`, Compiler Spec `default`, Raw Binary Base `0x00000000`.
- Image: `working/maincpu.bin`, `0x100000` Bytes, SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`.
- Ghidra-Block: `ram`, `0x000000-0x100000` (Endadresse exklusiv). Diese Blockbezeichnung ist Ghidras Loadername, kein Nachweis fuer physisches RAM.

MAME deklariert `maincpu` als `CODE_SIZE=0x400000` und mappt ROM bei `0x000000-0x3fffff`. Das rekonstruierte Image enthaelt davon `0x000000-0x0fffff`; der Bereich `0x100000-0x3fffff` hat in diesem Set keine Programm-ROM-Loads. Die Basis `0x0` ist durch MAME `ROM_LOAD16_BYTE` und `main_map` belegt, nicht geraten.

## MAME-CPU-Adresskarte

| CPU-Adresse | Groesse | MAME-Zuordnung | Quelle/Status |
|---|---:|---|---|
| `0x000000-0x3fffff` | 4 MiB | 68000 ROM (`main_map`) | MAME CPU-map; im gelieferten Set nur erste 1 MiB mit 8 Programm-ROMs geladen |
| `0x800000-0x800007` | 8 B | Input-Port `IN1` | MAME `main_map`; Spielerinput laut Portdefinition active-low |
| `0x800018-0x80001f` | 8 B | System-/DIP-Eingaben | MAME `main_map` |
| `0x800030-0x800037` | 8 B | Coin-Control writes | MAME `main_map`; Ghidra Resetcode schreibt `0x80`, dann `0` nach `0x800030` |
| `0x800100-0x80013f` | `0x40` | CPS-A custom registers | MAME `main_map` |
| `0x800140-0x80017f` | `0x40` | CPS-B custom registers (sf2eb-Regionbelegung) | MAME `main_map` |
| `0x800180-0x800187` | 8 B | Sound command latch | MAME `main_map` |
| `0x800188-0x80018f` | 8 B | Sound timer/fade latch | MAME `main_map` |
| `0x900000-0x92ffff` | `0x30000` | Graphics RAM | MAME `main_map`; Resetpfad loescht `0x900000-0x903fff` ab Ghidra `0x73c` |
| `0xff0000-0xffffff` | 64 KiB | 68000 Main RAM | MAME `main_map`; separater, nicht als Reset-erreichbar bestaetigter Clear-Block bei `0x752` |

Die MAME-Map ist in `src/mame/capcom/cps1.cpp` bei `main_map` und `cps1_10MHz` definiert. `sf2eb` verwendet dieses Maschinenprofil.

## `maincpu` ROM-Lanes

Alle acht Dateien sind je `0x20000` Bytes gross. `ROM_LOAD16_BYTE` setzt gerade Offset auf D15-D8 und ungeraden Offset auf D7-D0. Die Endadressen sind pro Lane inklusive:

| Paar | Gerade Lane / Datei | Ungerade Lane / Datei | gemeinsamer Bereich |
|---|---|---|---|
| P1 | `0x00000`, `sf2e_30b.rom`, Ende `0x03fffe` | `0x00001`, `sf2e_37b.rom`, Ende `0x03ffff` | `0x000000-0x03ffff` |
| P2 | `0x40000`, `sf2e_31b.rom`, Ende `0x07fffe` | `0x40001`, `sf2e_38b.rom`, Ende `0x07ffff` | `0x040000-0x07ffff` |
| P3 | `0x80000`, `sf2_28b.rom`, Ende `0x0bfffe` | `0x80001`, `sf2_35b.rom`, Ende `0x0bffff` | `0x080000-0x0bffff` |
| P4 | `0xc0000`, `sf2_29b.rom`, Ende `0x0ffffe` | `0xc0001`, `sf2_36b.rom`, Ende `0x0fffff` | `0x0c0000-0x0fffff` |

Die vier Paarbereiche sind zusammenhaengend und belegen `0x100000` Bytes ohne Luecke oder Ueberlappung. Einzelheiten und Checksummen stehen in `reports/rom_pair_mapping.csv` und `reports/reconstruction_report.md`.

## Weitere MAME-ROM-Regionen

| Region | Laenge | Belegung |
|---|---:|---|
| `gfx` | `0x600000` | 12 × `ROM_LOAD64_WORD`, je `0x80000`, drei zusammenhaengende 2-MiB-Bloecke |
| `audiocpu` | `0x18000` | `sf2_9.12a`: `0x00000-0x07fff`, `ROM_CONTINUE` nach `0x10000-0x17fff`; Loch `0x08000-0x0ffff` |
| `oki` | `0x40000` | zwei Samples je `0x20000`, zusammenhaengend |
| `aboardplds` | `0x0200` | fuenf `0x0117`-Byte-Eintraege an Offset `0` (ueberlappende MAME-Region-Loads) |
| `bboardplds` | `0x0200` | zwei `0x0117`-Byte-Eintraege an Offset `0` |
| `cboardplds` | `0x0200` | ein `0x0117`-Byte-Eintrag an Offset `0` |

Die acht PLDs aus dieser Definition fehlen im lokalen ROM-Verzeichnis. GFX-, Audio-, Sample- und PLD-Regionen sind getrennte MAME-Regions, nicht zusaetzliche 68000-Programmbytes.

## Vektoren und Datenstruktur-Kandidaten

- 68000 Vektorbereich: `0x000000-0x0000ff`, 64 Big-Endian-Longwords. SSP=`0x0`, Reset-PC=`0x40e`.
- Exception-/TRAP-Code: `0x400`, `0x402`, `0x40c`, Handlerbereich `0xb24-0xd34`.
- Runtime-Context-Kandidaten relativ zu A5: Records ab `A5-0x8000`, 8 Slots mit Stride `0x20` ab `A5-0x7f00`, Listenpointer/-zaehler bei `A5-0x7df8/-0x7df4`, Ringpufferdaten bei `A5+0xa2` mit Index `A5+0x1e`.
- Collider-Descriptor-Kandidat: Objektpointerfelder `A2+0x1a`/`A2+0x34`; Datenoffsets `+0,+2,+4,+6`; Facingflags `A2/A6+0x12`; Scratchwerte relativ zu `A5-0x7526..-0x7520`. Typ und Hitbox/Hurtbox-Zuordnung unbekannt.
