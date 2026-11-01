# Spielzustaende und Uebergaenge

## Ergebnisgrenzen

Im ROM befinden sich lesbare UI-/Spieltexte, darunter `STAGE 01`, `PLAYER=00`, `CREDIT=`, `INSERT COIN.`, `PUSH 1P START.`, `PUSH 1P OR 2P START.`, `PLAYER SELECT`, `ARE YOU MAN ENOUGH TO FIGHT`, `THE FIGHT IS ALL.` und `LET'S START OVER.`. Die Strings belegen vorhandene UI-/Statusdaten, aber nicht ihre Ausfuehrungsroutinen. Ghidras String-Data-Abfrage erfasste 14 relevante Strings und fand **0 Datenreferenzen** zu diesen Strings.

Darum konnten Titelbildschirm, Auswahl, Kampfstart, Sieg/Niederlage oder Rundenwechsel keiner Routine sicher zugeordnet werden. Die sichtbaren Texte werden nicht als Zustandsuebergangsbeweis behandelt.

## Sicherer Hardware-/Reset-Zustandsbefund

- Vektor- und Exception-Kontrollfluss ist im vorherigen Bericht `reports/control_flow.md` dokumentiert.
- `FUN_00083588` wird von Ghidra bei `0x83588` gefuehrt. Decompiler-Kontrollfluss: ruft `FUN_00001b1a`; prueft danach `A5+0x9e0`; im Nullpfad ruft es `FUN_000835c8`, `FUN_0008487e`, dann die beiden Dispatcher `FUN_00083838` und `FUN_000839ea`; im anderen Pfad `FUN_000835c8` und dieselben Dispatcher.
- Relevantes Feld: `A5+0x9e0` ist ein Flag, das auch in `FUN_00009314` und `FUN_00009476` Branches auswaehlt. Bedeutung des Flags ist unbekannt. Das ist ein belegter Subsystem-/Update-Dispatcher, aber kein sicherer Main-Loop oder globaler Spielzustand.
- Konfidenz: **hoch** fuer den dekompilierten Call-/Branch-Ablauf, **niedrig** fuer die Zuordnung zu Titel/Kampf/Spielzustand.

## Zustands-/Aktionsdispatcher (Vermutungen)

| Ghidra-Funktion | Beobachteter Kontrollfluss | Variablen / Referenzen | Einschaetzung |
|---|---|---|---|
| `FUN_00083838` @ `0x83838` | Switch ueber Byte `A6[2]`; 60 direkte Call-Sites; zahlreiche Case-Helper im Bereich `0x850ec-0x89fde`. Caller `FUN_00083588`. | `A6[2]`, Register `A0-A6`, `D0-D7`, Flags; Datenrefs u. a. `0xd8000`, `0xd8004`, Tabellen-/ROM-Adressen. | Vermutlich Objekt-/Befehlsdispatcher; **niedrig-mittel**, weil PCode Instruktionsueberlappungen meldet. Kein spezifischer Fighter-State bewiesen. |
| `FUN_000839ea` @ `0x839ea` | Switch ueber Byte `A6+1`; 80 direkte Calls; Caller ebenfalls `FUN_00083588`. | `A6+1`, Register `A0-A6`, `D0-D7`, Flags; ROM-Tabellenrefs inkl. `0x9ca`, `0x700`, `0x12da`, `0x9100`; GFX-RAM refs `0x90c514-0x90c596`. | Vermutlich zweiter Zustands-/Action-Dispatcher; **niedrig-mittel**. Decompiler meldet bad instruction data/ueberlappende Instruktionen. |
| `FUN_0007d6e2` @ `0x7d6e2` | Flagbedingungen, Calls `0x7e3f6`, `0x7e25e`, danach Felder setzen, `0x7d9b4`, Thunk `0x6314`, `0x7dcd6`. | `A5+0xade`, `A5+0x2e3`; `A2+0x151`, `+0x15a`, `+0x4c`, `+0x47`, `+0x54`, `+0x2a`, `+0x71`; `A3+8`, `A6+0x47/+0x12`. | Vermuteter Objekt-/Animationszustandsuebergang; **mittel** fuer Objektstatus, **niedrig** fuer globalen Spielzustand. |
| `FUN_0007d9b4` @ `0x7d9b4` | Zustands-Case-Pfade, 23 Calls. | A2/A3/A5-relative Statusbytes, u. a. Werte `2`, `4`, `0x63`, `0xa7`. | Objektzustandsroutine-Kandidat; **niedrig-mittel**, Grenzen/Instruktionsfluss teils fehlerhaft. |

## Hauptschleife und Stufen

Eine eindeutige Hauptschleife wurde nicht bestimmt. Der Resetpfad fuehrt statisch vom `JMP (A4)` bei `0x750` zum dekodierten LEA-Einstieg `0x426`; `0x42a` verzweigt weiter nach `0x7cc`. `0x428` ist das Extension-Word der LEA bei `0x426`. `FUN_00083588` ist ein plausibler periodischer Subsystem-Dispatcher, aber keine gesicherte Hauptschleife. Keiner der obigen Befunde benennt originale Titel-, Auswahl-, Kampf-, Sieg/Niederlage- oder Rundenlogik.

Keine im Ghidra-Projekt enthaltene Funktion wurde umbenannt. UI-Zustandszuordnungen bleiben offen, bis Stringreferenzen oder ein belastbarer Control-Flow-Pfad nachgewiesen werden.