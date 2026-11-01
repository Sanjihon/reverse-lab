# Charakter-, Animations- und Angriffslogik

## Identifikationsstand

Es wurde keine Routine sicher als Auswahl eines bestimmten Kaempfers, character-spezifische Physik, Spezialbewegung, Angriffsstart/-treffer, Hitstun, Guardstun, Siegerpose oder Animationsgraph identifiziert. Stringtreffer wie `PLAYER SELECT`, `ARE YOU MAN ENOUGH TO FIGHT` und `THE FIGHT IS ALL.` haben in Ghidras definiertem Dateninventar keine Xrefs. Keine Character-Funktion wurde erfunden oder umbenannt.

## Allgemeine Objekt-/Grafikroutinen

### `FUN_0007f1b4` - wahrscheinliche Sprite-/Objektlisten-Emission

- Adresse: `0x0007f1b4-0x0007f8a3`, Ghidra-Standardname bleibt bestehen.
- Kontrollfluss: prueft Objektflags, ruft `FUN_0007f8a4`/`FUN_0007f56a`, liest Sprite-/Tilebeschreibung, beruecksichtigt ein Budget und mehrere Flags, berechnet Koordinaten, verwirft/clipped Werte ausserhalb `0x000-0x1ff`, schreibt Vier-Wort-Descriptoren in einen Puffer.
- Variablen: `A5+0x2d7`, `A5+0x8a`, Budget `A5+0x2b6`, Koordinatenfelder `A5-0x7508`/`A5-0x7506`; Beschreibungsdaten ueber `A1+0x1a` und Zeiger auf A0/A3/A6. Calls `0x7f8a4`, `0x7f56a`, `0x7f62e`.
- Begruendung: koordinatenbasierte Cliptests und Sprite-Descriptor-Ausgabe sprechen fuer Rendering/Objektaufbereitung, nicht fuer die Spielereingabe selbst.
- Konfidenz: **mittel** fuer generische Sprite-/Objektlisten-Emission; **niedrig** fuer fighterbezogene Bedeutung.

### `FUN_000869ae` - Descriptor-Tabellen-Initialisierung

- Adresse: `0x000869ae-0x000869dd`; 18 eingehende Calls, hauptsaechlich von `FUN_00083838` und `FUN_000839ea`.
- Kontrollfluss: vier Eintraege, Schrittweite `0x40`; Wert aus ROM-Tabelle `0x869de + D0w*4`; schreibt `0x40|byte` und Nullfelder.
- Register/Variablen: `A0-A2,D0w,D3w,D7b,D7w,SP`; ROM-Quelle `0x869de`, Outputzeiger A0.
- Begruendung/Konfidenz: wiederholte Descriptorinitialisierung (**mittel**); konkrete Figur, Animation oder Tabelle unbekannt.

### `FUN_00009476` und `FUN_00009314` - GFX-/Layer-Aufbereitung

- `FUN_00009476` liegt bei `0x9476-0x98b7`. Sie schreibt GFX-Kommandomuster (`0x81ff`, `0x81de`, `0x81ee`) und verwendet Tabellen bei `0x96be`, `0x9582` sowie Helfer `0x935c`, `0x945a`, `0x9850`, `0x98b8`.
- `FUN_00009314` liegt bei `0x9314`; sie ruft je nach `A5+0x9e0` `FUN_000093d6`, `FUN_00009476`, `FUN_000096de`, `FUN_00011622` und `FUN_00005572` auf. Danach vergleicht sie D3w mit einer Tabelle im Bereich `0x1baa-0x1d72` und kann `FUN_0000207e` aufrufen.
- Begruendung/Konfidenz: GFX-/Layer-/Scene-Setup ist **mittel** wahrscheinlich. Keine direkte Charakter- oder Animationssemantik bewiesen.

## Gegner-/Action-Dispatcher-Kandidaten

`FUN_00083838` und `FUN_000839ea` werden durch `FUN_00083588` aufgerufen und dispatchen anhand Bytes in A6 (bei `A6[2]` bzw. `A6+1`) zu vielen lokalen Case-Helfern. Sie koennten Entity-, Aktions- oder Rendering-Befehle verarbeiten. Ghidra meldet aber Instruktionsueberlappungen und bad instruction data; weder Angriffs-IDs noch Character-IDs sind diesen Cases belastbar zugeordnet (**niedrig-mittel**).

`FUN_0007d6e2`/`FUN_0007d9b4` zeigen ebenfalls Zustandsbytes und Objektfelder; Belege reichen nur fuer einen Objektstatus-Uebergang, nicht fuer einen bestimmten Charakter oder Angriff.

## Spezialbewegungen und Angriffsablaeufe

Fuer Hadouken, Shoryuken, Tatsumaki oder andere Spezialbewegungen, Richtungsfolgen, Buttonkombinationen, Hitboxen, Schadenswerte, Block-/Trefferzustand und Animation-Frame-Tabellen liegt in dieser Phase keine sichere Zuordnung vor. Die MAME-Eingabeports definieren die Hardwarebits, aber die ROM-Analyse hat keine Routine mit diesen Bits als lesbare Quelle und keinem eindeutig identifizierten Angriffspfad verknuepft.