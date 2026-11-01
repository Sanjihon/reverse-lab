# Kollisionsanalyse

## Gesicherter Call-Pfad

Die Untersuchung lief im vorhandenen Ghidra-Projekt `working/SF2MainCPU.gpr` mit `68000:BE:32:default`, Basis `0x0`. Ghidra-Decompiler-Ausgaben liegen in `working/phase7_collision_helpers.log` und `working/phase7_collision_dispatch_final.log`; Call-Edges stammen aus `working/inventory_final.log`.

Der belegte Pfad ist:

```text
FUN_0007d6e2 @ 0x7d6e2
  call site 0x7d702 -> FUN_0007e25e @ 0x7e25e
    0x7e26a -> FUN_0007e420
    0x7e270 -> FUN_0007e486
    0x7e28c -> FUN_0007e430
    0x7e292 -> FUN_0007e486
    0x7e2ae -> FUN_0007e440
    0x7e2b4 -> FUN_0007e486
    0x7e2d8 -> table DAT_0007e2f6, selected by mask
    0x7e2e0 -> FUN_0007e450
    0x7e2e6 -> FUN_0007e486
```

`FUN_0007e25e` sammelt eine Maske aus Bits `1`, `2`, `4`, ruft pro gesetztem Bit einen Feld-/Achsen-Getter und `FUN_0007e486` auf. Bei einer nichtleeren Maske wird ein relativer Tabelleneintrag dispatcht; danach folgt ein weiterer Test mit `FUN_0007e450` und `FUN_0007e486`. Die Funktion liefert 1, wenn sie einen aktiven Kandidaten verarbeitet, sonst 0. Die genaue Bedeutung der Maskenbits ist aus dem Code allein nicht benannt.

## Geometrievergleich

`FUN_0007e486` berechnet zwei transformierte Positionen aus Objektreferenzpunkten und Descriptorfeldern. Die Ausrichtung/Facing-Flags bei `A6+0x12` und `A2+0x12` kehren jeweils einen horizontalen Offset um. Danach:

- X-Kandidaten: Positionen aus `A6+6` und `A2+6` plus transformierte `A3/A4`-Offsets; Vergleichs-Extents aus `A3[2] + A4[2]`.
- Y-Kandidaten: Positionen aus `A6+10` und `A2+10` plus `A3[1]`/`A4[1]`; Vergleichs-Extents aus `A3[3] + A4[3]`.
- Bei verletzter X-Bedingung kehrt die Funktion vor dem Y-Vergleich zurueck; andernfalls prueft sie auch Y. Das ist mit einer 2D-Axis-Aligned-Box-/Overlap-Pruefung vereinbar.
- Die Kandidatenpositionen werden in A5-relative Felder geschrieben: `A5-0x7526`, `A5-0x7524`, `A5-0x7522`, `A5-0x7520`.

**Konfidenz:** hoch fuer einen geometrischen X/Y-Overlap-Test mit Facing-Korrektur; mittel fuer die Interpretation als Collider-/Boxpruefung; niedrig fuer die Unterscheidung Angriffshitbox gegen Hurtbox. Die Variablen-/Descriptornamen bleiben Ghidras `FUN_...`/`DAT_...`; es wurden keine erfundenen Typen oder Namen angelegt.

## Feld- und Achsen-Getter

| Funktion | Ghidra-Adresse | beobachteter Zugriff | Konfidenz |
|---|---:|---|---|
| `FUN_0007e420` | `0x7e420` | liest `*(A2+0x1a)+8`; bei aktivem Flag nimmt sie ein 16-Bit-Feld ueber `*(A2+0x34)` bei Offset `0`. | hoch fuer Flag/Koordinatenpaar |
| `FUN_0007e430` | `0x7e430` | liest Descriptorbyte `+9`; Feld bei `A2+0x34+2`; liefert Maskenkandidat `2`. | hoch fuer beobachtete Operation |
| `FUN_0007e440` | `0x7e440` | liest Descriptorbyte `+10`; Feld bei `A2+0x34+4`; liefert Maskenkandidat `4`. | hoch fuer beobachtete Operation |
| `FUN_0007e450` | `0x7e450` | liest Descriptorbyte `+11`; Feld bei `A2+0x34+6`; liefert Wert `6`. | hoch fuer beobachtete Operation, Rolle unklar |

Diese Felder koennen optionale Kollisionsdimensionen/-parameter beschreiben. Ihre semantischen Namen (Breite, Hoehe, Tiefe oder andere Achsen-/Kontaktwerte) sind nicht sicher bestimmt.

## Kollisionsdispatch-Tabelle

Der Ghidra-Decompiler referenziert bei `0x7e2d8` die Tabelle `DAT_0007e2f6`. Die ersten acht Big-Endian-Offsets sind `0x10,0x14,0x22,0x30,0x56,0x64,0x8a,0xb0`; die Ziele sind Basis plus Offset. Damit ergeben sich fuer Maskenindex 0-7 die Ziele `0x7e306`, `0x7e30a`, `0x7e318`, `0x7e326`, `0x7e34c`, `0x7e35a`, `0x7e380`, `0x7e3a6`.

Ghidra-Funktionen wurden an diesen acht aus der Tabelle berechneten Zielen mit unveraenderten Standardnamen angelegt. Ihre Decompilertexte schreiben primär A5-relative Auswahl-/Ergebnisfelder `-0x7511` und `-0x7522`, wobei Vergleichswerte aus `-0x7532`, `-0x752e`, `-0x752a`, `-0x7538`, `-0x7536` und `-0x7534` verwendet werden. Das deutet auf Auswahl eines Ueberlappungs-/Trennachsenkandidaten, nicht auf belegte Schadensberechnung.

## Kollisionsreaktions-Caller

`FUN_0007d6e2` ruft `FUN_0007e25e` bei `0x7d702` auf und arbeitet in einem A5/A2/A3/A6-Kontext. Im selben Pfad werden unter Bedingungen Felder `A2+0x151`, `A2+0x15a`, `A2+0x4c`, `A2+0x47`, `A2+0x54`, `A2+0x2a`, `A2+0x71` sowie `A6+0x47` veraendert; danach folgen `FUN_0007d9b4`, ein Thunk bei `0x6314` und `FUN_0007dcd6`. Das ist ein belegter Kollisions-/Objektzustands-Callpfad, aber die Bedeutung von `0x78` und der veraenderten Felder ist nicht aufgeloest. Kein sicherer Beleg, dass dies ein Kaempfer-Treffer, Knockback oder Schaden ist.

## Nicht nachgewiesen

Keine separate Angriffsbox, Hurtbox, Trefferprioritaet, Invulnerabilitaetspruefung, Blockfront-/Richtungspruefung oder fighter-spezifische Hitbox-Tabelle ist sicher identifiziert. Der Befund ist eine generische geometrische Ueberlappungs-/Kollisionsroutine mit einem zustandsabhaengigen Caller.