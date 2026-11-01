# Schadensberechnung und Trefferreaktion

## Ergebnis

In den gezielt dekompilierten Kollisions-, Bewegungs-/Objektzustands- und Sprite-Helfern wurde keine belastbare HP- oder Schadensformel gefunden. Es gibt keinen durch Xrefs belegten Pfad von `FUN_0007e25e` zu einer als HP-/Schadensspeicher identifizierten globalen Variable. **Schaden, Guard-Reduktion, Trefferstun, Knockbackgeschwindigkeit und Rundensiegbedingungen sind in dieser Phase nicht aufgeloest.**

## Relevante, aber nicht als Schaden belegte Felder

| Ghidra-Stelle | Beobachtung | Warum nicht als Schaden etikettiert |
|---|---|---|
| `FUN_0007e25e` @ `0x7e25e` | Absolutdifferenz aus D3w wird in `A5-0x7538`, `-0x7536`, `-0x7534` abgelegt; Maske `A5-0x751a` sammelt Bits `1/2/4`. | Das sind Kollisions-/Extents-/Achsendifferenzen; keine HP- oder Schadensoperation erkennbar. |
| Dispatchziele `FUN_0007e30a` bis `FUN_0007e3a6` | Schreiben einen Auswahlwert bei `A5-0x7511` und einen Kandidatenwert bei `A5-0x7522` aus `-0x7532`, `-0x752e`, `-0x752a` bzw. Vergleich der Differenzen. | Sie waehlen einen Overlap-/Trennkandidaten; keine Schadenskonstante oder Lebenspunkteaenderung sichtbar. |
| `FUN_0007d6e2` @ `0x7d6e2` | Setzt unter Bedingungen u. a. `A2+0x151=0x78`, `A2+0x15a=1`, kopiert Bytes nach `A2+0x4c/+0x54`, dekrementiert `A2+0x2a`, ruft `FUN_0007d9b4`/`FUN_0007dcd6`. | Zustands-/Objektreaktion plausibel, aber Feld `A2+0x2a` ist ohne Struktur-/Xref-Aufloesung nicht als HP zu identifizieren. `0x78` kann ein State-/Modewert sein. |
| `FUN_0007f1b4` @ `0x7f1b4` | Budget bei `A5+0x2b6` wird bei Objekt-/Descriptorverarbeitung geprueft und veraendert. | Aus dem Sprite-/Listen-Ausgabepfad; kein HP- oder Schadensbeleg. |

## Benoetigte weitere Evidenz

Eine Schadenszuordnung braucht einen durchgaengigen Call-/Data-Xref-Pfad von einem Treffer-/Attack-Event zu einem bestaetigten Lebenspunktefeld und zu einer Anzeige-/Rundenbedingung. Ein solcher Pfad wurde hier nicht gefunden. Keine Variable wurde als `HP`, `damage` oder aehnliches umbenannt.