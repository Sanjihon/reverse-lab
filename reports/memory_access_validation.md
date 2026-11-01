# Phase 11: Speicherzugriffsvalidierung

## Reset-Prefix

| Originalzugriff | C-Entsprechung | Ergebnis |
|---|---|---|
| Byte `0x80`, dann `0x00` nach `0x800030` | `volatile u8`-Schreibzugriffe nach `SF2_COIN_CONTROL_START` | Adresse, Breite, Wert und Reihenfolge stimmen mit der MAME-Speicherkarte und Ghidra-Instruktionen ueberein. |
| Wortschreiben ab `0x900000`, Postincrement um 2; Vergleich gegen `0x903fff` | Wort-Store pro `u32`-Adresse bis `address <= SF2_GFX_RESET_CLEAR_END` | Effektive Write-Adressen sind `0x900000, 0x900002, ..., 0x903ffe`; kein ungerades Wort und keine Luecke. |
| A4-/PC-relative Steuerung und `JMP (A4)` | nicht in C dargestellt | Keine Aussage ueber Hostpointer oder Folge-Globals. |

`maincpu.bin` und ROM sind nur Eingaben der Analyse. Der Reset-C-Code wurde nicht gegen MMIO ausgefuehrt.

## Collision-Zugriffe

| Rolle aus dem Assembler | 68000-Zugriff | C-Adapter | Status |
|---|---|---|---|
| Objektreferenz x | Wort an `(A6+6)`, `(A2+6)` | zwei Big-Endian-Wortloads | Offset und Wortbreite beobachtet; Feldname unbekannt. |
| Objektreferenz y | Wort an `(A6+10)`, `(A2+10)` | zwei Big-Endian-Wortloads | Offset und Wortbreite beobachtet; Feldname unbekannt. |
| horizontale Transformationsflags | Byte an `(A6+0x12)`, `(A2+0x12)` | Byte ungleich null | Zugriff beobachtet; Facing-/Flip-Interpretation plausibel, nicht als Structfeld gesichert. |
| A3/A4 Descriptor | Bytes `+0,+1,+2,+3,+5` | sechs-Byte-Hostpointer | konkrete Lesebreiten/Offsets beobachtet; Datensatzlaenge und Feldsemantik bleiben unbekannt. |
| Kandidatenpositionen | Wortstores `A5-0x7526`, `-0x7522`, `-0x7524`, `-0x7520` | vier `s16 *`-Outputs | numerische Werte vergleichbar; Ausgabe-ABI abgeleitet. |
| temporare Deltas/Ergebnis | D2, D3, D4 | `delta_x_out`, `delta_y_out`, `bool` | Register nicht in Speicher umgedeutet; C-API ist Testadapter. |

Es gibt keine durch diese beiden Modelle bewiesenen absoluten C-globalen Spielvariablen. A5-relative Scratch-Speicher sind relativ zum Laufzeitregister und keine globale Adresse. `src/memory/memory_map.h` enthaelt MAME-verifizierte Adresskonstanten; es bildet keinen Hostspeicher ab.

## Struktur-, Endian- und Alignmentgrenzen

- Die Testvektoren pruefen Big-Endian-Werte `0x0000`, `0x7fff`, `0x8000`, `0xffff`; die C-Laderoutine liest zwei Bytes und benoetigt keine Host-Wortausrichtung.
- Die Originalzugriffe mit `MOVE.W` setzen fuer gueltigen 68000-Zugriff gerade effektive Adressen voraus. Offsets +6/+10 sind gerade, aber die Ausrichtung von A2/A6-Basen ist unbekannt. Verhalten bei ungerader Basis ist eine moegliche Abweichung: byteweises C liest weiter, 68000 kann Address Error ausloesen.
- Keine verifizierten Player-, Character-, Move-, Animation-, Hurtbox-, Hitbox- oder Game-State-Strukturen; Feldgroessen jenseits der tatsaechlich gelesenen Bytes/Worte sind UNKNOWN.
- Es wurden keine RAM-Tabellen, unbekannten Descriptorlaengen oder Inputports in C nachgebildet. Die statische Xref-Suche fand keine Referenz in IN1/DIP-Fenster, schliesst dynamische Adressberechnung jedoch nicht aus.

## Integritaet

Waerend Phase 11 blieb `working/maincpu.bin` bei SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`. Alle 23 vorhandenen Original-ROMs stimmen mit `reports/rom_manifest.csv` ueberein. Ghidra-Metadaten `project.prp` und die Programmdatenbank behielten ihre vor/nach den Read-only-Exports geprueften Hashes. Kein Original wurde geschrieben.
