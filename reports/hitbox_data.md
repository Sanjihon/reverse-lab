# Kollisions- und Boxdaten

## Allgemeiner Descriptor-Kandidat

`FUN_0007e420`, `FUN_0007e430`, `FUN_0007e440` und `FUN_0007e450` lesen optionale Bytes aus einem Descriptor, auf den `*(A2+0x1a)` zeigt, und greifen bei gesetztem Byte auf einen Datenblock ueber `*(A2+0x34)` zu:

| Getter | Aktivierungsbyte im Descriptor | 16-Bit-Feld im Datenblock | Rueckgabemarker |
|---|---:|---:|---:|
| `FUN_0007e420` @ `0x7e420` | `+8` | `+0` | aktiv=`1`, inaktiv=`0` |
| `FUN_0007e430` @ `0x7e430` | `+9` | `+2` | aktiv=`1`, inaktiv=`2` |
| `FUN_0007e440` @ `0x7e440` | `+10` | `+4` | aktiv=`1`, inaktiv=`4` |
| `FUN_0007e450` @ `0x7e450` | `+11` | `+6` | aktiv=`1`, inaktiv=`6` |

Die Markerwerte `2` und `4` werden in `FUN_0007e25e` als Bitmaske mit `1` kombiniert. Das stuetzt einen generischen achsen-/Komponenten-Auswahlmechanismus. Es ist nicht belegt, ob der Block Angriffshitbox, Hurtbox, Bodenkontakt, Wandkontakt oder mehrere Colliderklassen beschreibt.

## Zwei-Objekt-Bounds

`FUN_0007e486` (@ `0x7e486`) kombiniert zwei Datenbeschreibungen `A3` und `A4` mit zwei Objektpositionen aus `A6` und `A2`:

- Objektpositionen: `A6+6` / `A2+6` als horizontale Basis; `A6+10` / `A2+10` als zweite Koordinate.
- Facing-/Orientierungsflags: `A6+0x12` und `A2+0x12` spiegeln horizontale Descriptoroffsets.
- Horizontale Descriptorwerte: `A3[5]` oder `*A3`; `*A4`; Extent-Summe `A3[2]+A4[2]`.
- Zweite Koordinate: Offsets `A3[1]`/`A4[1]`; Extent-Summe `A3[3]+A4[3]`.
- Scratch-/Zwischenwerte werden in `A5-0x7526`, `-0x7524`, `-0x7522`, `-0x7520` gespeichert. Der Code kehrt frueh zurueck, wenn die X- bzw. anschliessende Y-Ueberlappungsbedingung fehlschlaegt.

**Interpretation:** stark vereinbar mit einer 2D-Box-/Overlap-Pruefung (**hoch** fuer geometrische Ueberlappung; **niedrig-mittel** fuer konkrete Hitbox/Hurtbox-Semantik). Die Boxseiten-/Extentfelder sind kandidatenweise abgeleitet, nicht als Typen von Ghidra erkannt.

## Masken-/Dispatch-Tabelle

Bei `0x7e2d8` referenziert `FUN_0007e25e` `DAT_0007e2f6`. Die ersten Big-Endian-Worte sind relative Offsets:

| Maskenindex | Tabellenoffset | berechnetes Ziel | beobachtete Wirkung |
|---:|---:|---:|---|
| 0 | `0x0010` | `0x7e306` | liefert `0` |
| 1 | `0x0014` | `0x7e30a` | setzt Auswahl `0`; kopiert Kandidat `-0x7532` nach `-0x7522` |
| 2 | `0x0022` | `0x7e318` | setzt Auswahl `2`; kopiert `-0x752e` |
| 3 | `0x0030` | `0x7e326` | waehlt zwischen 1/2 anhand `-0x7536` gegen `-0x7538` |
| 4 | `0x0056` | `0x7e34c` | setzt Auswahl `4`; kopiert `-0x752a` |
| 5 | `0x0064` | `0x7e35a` | waehlt zwischen 0/4 anhand `-0x7534` gegen `-0x7538` |
| 6 | `0x008a` | `0x7e380` | waehlt zwischen 2/4 anhand `-0x7534` gegen `-0x7536` |
| 7 | `0x00b0` | `0x7e3a6` | waehlt zwischen 0/2/4 anhand der drei Differenzen |

Die Handler wurden an den aus der Tabelle berechneten Adressen mit Ghidra-Standardnamen angelegt; keine semantischen Namen wurden erfunden.

## Call- und Datenreferenzen

- `FUN_0007d6e2` ruft `FUN_0007e25e` bei `0x7d702` auf.
- `FUN_0007e25e` ruft die Getter/Comparatoren an den im Bericht `collision_analysis.md` dokumentierten Call-Sites auf und referenziert `DAT_0007e2f6` bei `0x7e2d8`.
- Der Kollisionspfad schreibt nur in A5-relative Scratch-/Statusfelder. Es wurde keine globale Angriffs- oder Hurtboxtabelle mit sicherem Typ gefunden.
- `FUN_0007f1b4` erzeugt Render-/Objektlisteneintraege aus A0/A1/A3/A6-Descriptoren. Diese Renderingdaten sind nicht automatisch die Kollisionsboxdaten.

## Offene Semantik

Nicht entschieden sind: welche Descriptorquelle Angriff und welche Verteidigung repraesentiert; ob ein Kollisionsbit Boden/Wand/Objekt oder Hitboxachse codiert; Einheiten/Skalierung der Koordinaten; ob A2/A6 Kaempfer oder andere Spielobjekte sind; und wie eine erkannte Ueberlappung auf Schaden/Knockback abgebildet wird. Siehe [damage_calculation.md](damage_calculation.md) fuer die getrennte Schadensbewertung.