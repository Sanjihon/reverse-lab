# Phase 9: Datenstrukturen und Typen

## Feste Ganzzahltypen

`src/common/types.h` bildet die benoetigten 68000-Wortbreiten auf `<stdint.h>`-Typen ab und enthaelt `_Static_assert`-Pruefungen fuer Byte-/Wortgroessen und 8-Bit-Bytes. Der Host-Compiler-Test ergab `u8/s8=1`, `u16/s16=2`, `u32/s32=4` Byte.

`u32`/`s32` bleiben auf dem Host 32 Bit. Die 68000-Zieladresse ist in diesem Projekt 24-Bit-physisch; ein `u32`-Adresswert ist eine Speicherabbilddarstellung, kein Beleg fuer einen 32-Bit-externen Bus.

## Direkt dargestellte Datenstruktur

### Verifizierter Vektoreintrag

`src/system/interrupts.c` enthaelt den Typ `sf2_verified_vector` mit `u16 vector_index` und `u32 target_address` sowie eine Liste der 36 aus Ghidras 256 gelesenen Vektorbytes nichtreservierten Werte. Die Werte stammen aus Big-Endian-Longwords bei `4 * vector_index`.

Der C-Record enthaelt bewusst den Index als Metadatum. Er ist **nicht** als gepacktes Byte-fuer-Byte-Layout der Vektortabelle zu verwenden; Host-Struct-Padding ist moeglich. Reservierte Eintraege `0xffffffff` sind aus der Liste ausgelassen und in `reports/vector_analysis.md` dokumentiert.

## A5-relative Runtime-Kontexte (Kandidaten)

Die TRAP-Handler zeigen tatsaechliche Zugriffe auf A5-relative Records und Listen. Mangels sicherer A5-Absolutbasis werden diese Felder in Phase 9 nicht als C-Struct implementiert.

| Ghidra-Adressen | Beobachtete Felder | Strukturhypothese / Status |
|---|---|---|
| `0xb24`, `0xbe4` | `A5-0x8000 + D0`; Status bei Slotbasis, Felder `+0x04`, `+0x10`, `+0x12`, `+0x14`, `+0x18`, `+0x1c` | Indizierte Record-Tabelle, Feldsemantik unbekannt; Zugriffsmuster **CONFIRMED**, Recordtyp **PROBABLE**. |
| `0xc12`, `0xc60` | acht Slots, Basis `A5-0x7f00`, Stride `0x20`, Statusbyte und Offset `+0x16` | Slot-/Objektpool oder Freiliste; **PROBABLE**, kein Struct in C. |
| `0xb62`, `0xbb0`, `0xc12` | Liste/Zaehler bei `A5-0x7df8` und `A5-0x7df4` | Pointerstack/Freiliste; **PROBABLE**. |
| `0x21e2` | Datenbasis `A5+0xa2`, Index `A5+0x1e`, Indexmaske `0xfc` | 256-Byte-Ringpuffer-Kandidat; Mechanik **CONFIRMED**, Zweck **UNKNOWN**. |
| `0x7e25e`, `0x7e486`, `0x7e492` | Scratch `A5-0x7538/-0x7536/-0x7534`, Kandidaten `-0x7532/-0x752e/-0x752a`, Ergebnis `-0x7522`, Flag `-0x751a` | Collider-/Overlap-Rechenkontext; Offsets/Operationen **CONFIRMED**, semantisches Struct **PROBABLE**. |
| `0x7d6e2`, `0x7d9b4`, `0x83588`, `0x83838`, `0x839ea` | zahlreiche A2/A3/A5/A6-relative Statusbytes und Unterzustaende | Objekt-/Dispatch-Kontextkandidaten; kein belastbares Player- oder Fighter-Struct. |

## Memory-mapped I/O und RAM

`src/memory/memory_map.h` enthaelt MAME-verifizierte Adresskonstanten fuer ROM, `IN1`, DIP, Coin-Control, CPS-A/B-Register, Sound-Latches, Graphics RAM und Main RAM. Diese Konstanten sind Adresswerte aus MAME, keine C-Pointer auf hostseitig gemappten Speicher. `IN2`-Kickmasken sind vermerkt, aber ohne behauptete CPU-Adresse.

## Nicht rekonstruierte Strukturen

Keine `Player`, `Character`, `Move`, `Animation`, `Hitbox`, `Hurtbox`, `Damage`, `Round` oder `GameState`-C-Struktur wurde erzeugt. Die tatsaechlichen Feld-Offsets sind in den Phase-1-bis-8-Berichten belegt, aber ihre semantischen Feldtypen reichen nicht fuer eine belastbare C-Struktur. `collision.c` akzeptiert daher Bytepointer und erklaert die Descriptoroffsets, ohne ein ABI-Struct vorzutaueschen.

**Anzahl:** 1 direkt dargestellter Recordtyp (Vektorindex/Ziel); 0 belastbar rekonstruierte Runtime-/Spieler-/Charakterstrukturen. Collider-/Recordlayouts bleiben Kandidaten und sind nicht als C-Struct implementiert.