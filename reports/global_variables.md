# Globale Daten und RAM-Kandidaten

Untersucht wurden Ghidra-Datenreferenzen aus dem persistenten Projekt. Das importierte Image belegt nur `0x000000-0x0fffff`; die MAME-CPU-Karte weist zusaetzlich I/O, GFX-RAM und Main-RAM ausserhalb dieses Raw-Images aus. Es wurden keine globalen Variablen in Ghidra umbenannt oder mit erfundenen Typen versehen.

## Referenzinventar

Das Ghidra-API-Inventar enthaelt 975 Datenreferenzen:

| Zielklasse | Referenzen | Interpretation |
|---|---:|---|
| ROM / Konstante / Tabelle innerhalb `0x000000-0x0fffff` | 867 | Code-nahe Konstanten, Switch-/Lookup-Tabellen und sonstige ROM-Ziele; die automatische Typisierung ist nicht vollstaendig verlaesslich. |
| CPS-I/O `0x800000-0x8001ff` | 8 | Unter anderem Coin-Control `0x800030`; reset code schreibt hierhin. |
| Graphics RAM `0x900000-0x92ffff` | 81 | Direkte Zugriffe/Initialisierung von GFX-RAM. |
| Stack-Symbole/-Offsets | 18 | Vom Ghidra-Stackframe abgeleitete lokale Referenzen, keine globalen RAM-Adressen. |
| Sonstige/unaufgeloeste Ziele | 1 | Nicht sicher klassifiziert. |

Es gab im Ghidra-Referenzinventar **keine eindeutig absolute Datenreferenz** in MAMEs Main-RAM-Bereich `0xff0000-0xffffff`. Das beweist nicht, dass kein Main-RAM benutzt wird: viele Zugriffe sind register-/pointer-relativ und werden nicht als absolute globale Adresse aufgeloest.

## Belegte Hardware- und RAM-Adressen

- `0x800030`: MAME-Coin-Control; Resetcode schreibt `0x80` und anschliessend `0x00`.
- `0x900000-0x903fff`: vom Reset-Initialisierungsblock bei `0x73c` geloeschter GFX-RAM-Bereich.
- `0x900400`: Ziel eines tabellengesteuerten GFX-Initialisierungsblocks bei `0x7cc`.
- `0x910000` und `0x90c000`: weitere im Ghidra-Listing als GFX-RAM-Ziele auftretende Initialisierungsbereiche.
- `0xff0000-0xffffff`: laut MAME Main-RAM; ein separater Listingblock bei `0x752` schreibt den Bereich mit Null-Longwords. Reachability aus dem Resetpfad ist wegen indirekter Spruenge nicht bestaetigt.

## A5-relative globale Zustandskandidaten

Mehrere Trap-Handler behandeln `A5` als Basis eines Runtime-Kontexts. Ghidra kann den absoluten Wert von A5 nicht stabil bestimmen; folgende Offsets bleiben daher Struktur-/Global-Kandidaten, keine benannten Variablen:

| Relative Adressen | Beobachtetes Muster | Interpretation / Konfidenz |
|---|---|---|
| `A5-0x8000 + D0` | TRAP #0 initialisiert Status `0x0c00`, A0 sowie D1/D2-Felder; TRAP #2 loescht Felder desselben Eintrags. | Indizierte Record-Tabelle; **hoch** fuer Zugriffsmuster, Typ unbekannt. |
| `A5-0x7f00`, Stride `0x20`, 8 Slots | TRAP #8 sucht anhand Statusbyte und Feld `+0x16`, leert den Slot und legt den Pointer auf eine Liste. | Slot-/Objektpool; **mittel-hoch**. |
| `A5-0x7df8`, `A5-0x7df4` | Ein Handler poppt einen Pointer und dekrementiert den Zaehler; ein anderer pusht Pointer und inkrementiert. | Freiliste oder Ringstruktur; **mittel**. |
| `A5+0xa2`, Index bei `A5+0x1e` | `FUN_000021e2` schreibt D0/D1 in einen Eintrag und maskiert den Index mit `0xfc`. | 256-Byte-Ringpuffer-Kandidat; **mittel-hoch**. |
| `A5+0x2d7`, `A5+0x8a`, `A5+0x2b6`, `A5+0x9e0`, `A5+0x818`, `A5+0x81c` | Decompiler zeigt Status-/Flagtests, Counter/Value-Felder und Updatepfade in Kandidatenfunktionen. | Zustands-/Objektfelder; Zuordnung ohne stabilen A5-Basiswert **niedrig-mittel**. |

## ROM-Tabellen

Am haeufigsten referenzierte ROM-Ziele des Inventars sind `0x9ca` (20 Referenzen), `0x8000` (18), `0x8004` (18), `0x8006` (17), `0x9e0` (10), `0x526` (10), `0x826` (10), `0x6cc` (8), `0x657` (7), `0x957` (7) und `0x800e` (7). Diese Adressen liegen im importierten ROM, nicht in der MAME-I/O-Region `0x800000+`. Ihre Datenart ist nicht fuer alle Ziele bestimmt.