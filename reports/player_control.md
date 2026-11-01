# Spielersteuerung und Eingabepfade

## Beleglage und Abgrenzung

Untersucht wurde das gespeicherte Ghidra-Projekt `working/SF2MainCPU.gpr` (Ghidra 12.1.4, `68000:BE:32:default`, Basis `0x0`) sowie MAMEs `sf2eb`-Adress- und Input-Port-Definition. Die ROM-Bytes im Ghidra-Projekt sind `working/maincpu.bin`.

MAME ordnet `IN1` auf CPU-Adressen `0x800000-0x800007` und DIP-Eingaenge auf `0x800018-0x80001f` ab. `sf2` inkludiert `cps1_6b`; dessen Bitbelegung kommt aus dem geerbten `cps1_3b`-Port plus den sechs zusaetzlichen Kick-Buttons ([CPS1 Input-Definitionen](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L840-L954), [`sf2`-Input-Set](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L2047-L2090), [CPU-Adresskarte](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L577-L599)). Diese MAME-Portzuordnung ist ein Hardwarebeleg, aber kein Nachweis, dass eine bestimmte SF2-Funktion die Eingaben liest.

## MAME-Hardwarebelegung

Alle Signale sind active-low: gesetzter Bitwert `0` bedeutet gedrueckt/aktiv, `1` bedeutet nicht aktiv. `IN1` ist das 16-Bit-Spielerport; P1 belegt das niedrige Byte, P2 das hohe Byte.

| Spieler | Signal | Maske in `IN1` |
|---|---|---:|
| P1 | Rechts / Links / Unten / Oben | `0x0001` / `0x0002` / `0x0004` / `0x0008` |
| P1 | Punch 1 / 2 / 3 | `0x0010` / `0x0020` / `0x0040` |
| P2 | Rechts / Links / Unten / Oben | `0x0100` / `0x0200` / `0x0400` / `0x0800` |
| P2 | Punch 1 / 2 / 3 | `0x1000` / `0x2000` / `0x4000` |
| P1 / P2 | unbenutzte Bits im `IN1`-Port | `0x0080` / `0x8000` |

Der geerbte CPS1-6-Button-Port `IN2` deklariert zusaetzlich:

| Spieler | Kick-Buttons | Masken in MAME-Port `IN2` |
|---|---|---:|
| P1 | Short / Forward / Roundhouse | `0x01` / `0x02` / `0x04` |
| P2 | Short / Forward / Roundhouse | `0x10` / `0x20` / `0x40` |

`IN2` ist hier nur als MAME-Portdefinition dokumentiert; aus der untersuchten CPS1-Hauptadresskarte wird keine direkte 68000-Adresse fuer diese Zusatzbits abgeleitet. Die Richtungsdefinition ist 8-way. Aus MAMEs Eingabedefinition folgt keine konkrete Spielroutine fuer Laufen, Springen oder Ducken.

## ROM-Befunde zur Eingabe

- Das Ghidra-Referenzinventar enthaelt keine Datenreferenz aus einer Funktion nach `0x800000-0x800007` oder `0x800018-0x80001f`.
- Eine Suche nach absoluten Big-Endian-Longword-Literalen dieser Portadressen im Image ergab 102 Treffer. Ghidras Codeunit-Pruefung klassifizierte 11 Treffer als Instruktionsueberdeckung; jeder dieser Instruktions-Treffer ist `SUBA.L #0x8000,A0`, nicht ein Zugriff auf die Input-Portadresse. 91 Treffer liegen in definierten Dateneinheiten.
- Die vom Ghidra-Stringinventar erfassten 14 UI-Strings haben null eingehende Referenzen. Rohe Texttreffer wie `PLAYER SELECT`, `PUSH 1P START` oder `THE FIGHT IS ALL.` sind damit keine belegten Kontrollflussanker.
- **Ergebnis:** Keine sichere Funktion fuer P1/P2-Inputpolling, Richtungsabfrage oder Punch-/Kick-Auswertung identifiziert. Ein indirekt aufgebauter Input-Zeiger waere moeglich, wurde aber nicht belegt.

## Ringpuffer-Kandidat `FUN_000021e2`

- Ghidra-Adresse: `0x000021e2-0x00002223` (66 belegte Bytes); Name bleibt `FUN_000021e2`.
- Kontrollfluss: prueft drei A5-relative Kontrollworte; bei nicht passender Kombination schreibt sie D0w/D1w an `A5+0xa2+Index`, erhoeht den Index und maskiert ihn mit `0xfc`.
- Register: liest `A4,A5,D0w,D1w,D2w,SP,ZF`; schreibt `A4,D2w,SP` und Flags. 17 direkte Callers, keine ausgehenden direkten Calls.
- Beleg und Konfidenz: klarer begrenzter Ringpuffer-Schreibzugriff (**hoch** fuer Ringpuffermechanik). Die Verbindung zu Spieler-Eingaben, Aktionsereignissen oder einem anderen Eventtyp ist nicht nachgewiesen (**niedrig** fuer Inputbedeutung).

## Verwechslung vermeiden

`FUN_00001b1a` referenziert `0x80010c`, `0x80010e` und `0x800154`; sein Decompiler zeigt Schreibzugriffe auf CPS-A/B-Steuerregister und ein Initialisieren von GFX-RAM `0x90d780`. Es ist eine Hardware-/Grafikinitialisierung, keine belegte Spieler-Eingaberoutine.

Die meisten sonstigen Eingabefragen bleiben offen: Register-/Pointer-relatives Polling, Entprellung, P1/P2-Weiterleitung und Eingabepufferzuordnung konnten nicht sicher zurueckverfolgt werden. Funktionen wurden nicht umbenannt.