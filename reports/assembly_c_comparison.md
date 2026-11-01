# Phase 11: 68000-Assembler-/C-Vergleich

## Grundlage

Ghidra 12.1.4, Language `68000:BE:32:default`, Image Base `0x0`. Verwendete Belege: `working/phase9_source_evidence.log`, `working/phase9_decompiler.log`, `working/phase10_inventory.log`, `working/phase10_source_evidence.log` und `reports/function_coverage.csv`. Es wurde kein neuer Autoanalyse-Lauf gestartet.

## Reset-Prefix: `0x40e`

| Assemblerbeleg | C-Entsprechung | Bewertung |
|---|---|---|
| `0x40e: MOVE.B #0x80,(0x800030).L` | `coin_control[0] = 0x80` | Wert und Bytebreite stimmen. |
| `0x416: MOVE.B #0,(0x800030).L` | `coin_control[0] = 0` | Wert und Reihenfolge stimmen. |
| `0x41e: LEA (0x6,PC),A4`, DATA-Xref auf `0x426` | nicht modelliert | Bewusste Grenze des Prefixes. |
| `0x422: BRA.W 0x73c` | nicht modelliert | C-Funktion bildet Kontrollfluss nicht ab. |
| `0x73c/0x742`: A0=`0x900000`, A1=`0x903fff` | Schleifenstart `0x900000`, Grenze `0x903fff` | Initialwerte stimmen. |
| `0x748: MOVE.W #0,(A0)+`; `0x74c: CMPA.L A1,A0`; `0x74e: BLS 0x748` | `volatile u16` schreiben, Adresse um 2 erhoehen, solange `address <= 0x903fff` | Schreibt gerade Worte von `0x900000` bis einschliesslich `0x903ffe`, wie der Postincrement-Loop. |
| `0x750: JMP (A4)` | nicht modelliert | Kein Sprung, Registerzustand oder Folgezustand wird als C-Verhalten behauptet. |

Es gibt keine C-Funktionsaufrufe oder globalen C-Variablen in diesem Prefix. Die C-Funktion ist fuer den beschriebenen Side-Effect-Bereich **CONFIRMED**, nicht fuer den gesamten Ghidra-Body.

## Collision-Modell: `0x7e486` / `0x7e492`

`0x7e486` ist ein 12-Byte-Einstiegsfragment (`0x7e486..0x7e491`), das bedingt in den separaten 156-Byte-Body `0x7e492..0x7e52d` verzweigt. Der gemeinsame Hostwrapper ist eine PROBABLE-Kontrollflussrekonstruktion.

| 68000-Operation | C-Modell | Vergleich |
|---|---|---|
| `MOVE.B` aus A3-Descriptor; `BPL`; ggf. `NEG.W`; bei positivem Byte `EXT.W` | Bit-7-Zweig: 16-Bit-Negation des unsigned Bytewerts; sonst sign-extend von Descriptorbyte 0 | Gepruefte Byte-/Wortbitmuster stimmen, einschliesslich `0xfe -> -254` im markierten Pfad. |
| `TST.B (0x12,A6/A2)` und bedingtes `NEG.W` | Spiegelung des horizontalen Offsets bei nichtnull Flagbyte | Bedingung und 16-Bit-Wraparound stimmen. Die Flagsemantik bleibt unbenannt. |
| `ADD.W (6,A6/A2),D0/D2` | Big-Endian-Wort laden und mit 16-Bit-Wrap addieren | Niedrige 16 Bit stimmen; C liest zwei Bytes in Big-Endian-Reihenfolge. |
| `MOVE.W` nach A5-Offsets `-0x7526`, `-0x7522`, `-0x7524`, `-0x7520` | vier explizite `s16 *`-Ausgaben fuer x/y-Kandidaten | Werte sind abgebildet; Hostpointer sind kein rekonstruierter A5-ABI. |
| `EXT.W` fuer Descriptorbytes `+2/+3`, `ADD.W` der Extents | `sign_extend_byte` und `add16` | Vorzeichen- und Wortbreitenverhalten stimmen. |
| `SUB.W`, `ADD.W`, `ADD.W` zum Verdoppeln, `CMP.W`, `BHI` | wrap-around `sub16`/`add16`, anschliessend vorzeichenloser `u16`-Vergleich | Die BHI-unsigned-Bedingung entspricht `>` ueber die 16-Bit-Ergebnisse, auch bei Wraparound. |
| `TST.W`/`BMI`, optional `NEG.W`, anschliessend `ADD.W` | Auswahl `delta < 0 ? +extent : -extent` mit Wort-Wrap | Gepruefte Rand-/Vorzeichenfaelle stimmen. |
| gleiche Folge fuer Y | analoge Y-Berechnung | Gepruefte Touch-/Abbruchfaelle stimmen. |
| Erfolg: `MOVEQ #1,D4; RTS`; Fehler: `MOVEQ #0,D4; RTS` | `true`/`false` | Logischer Wert stimmt; D4 und Condition Codes werden nicht als 68000-ABI implementiert. |

### Arithmetik und Representation

- Relevante Originaloperationen sind `MOVE.B`, `EXT.W`, `NEG.W`, `ADD.W`, `SUB.W`, `CMP.W`, `TST.B`, `TST.W`, `BPL`, `BMI` und `BHI`. In den zwei gemappten Modellen gibt es keine zu vergleichende 32-Bit-Datenarithmetik oder Speicher-Schiebeoperation.
- `add16`, `sub16` und `negate_s16_wrap` bilden Wort-Wraparound ueber `u16` ab. `s16_from_bits` wandelt die 16-Bit-Bitmuster ohne ausserhalb des signed Zielbereichs liegende C-Casts um. `sign_extend_byte` bildet `EXT.W` nach.
- `load_s16_be` entspricht der Big-Endian-Bytefolge fuer ein Wort. Die tatsaechlichen 68000-Wortzugriffe an `A2/A6 + 6/+10` sind gerade Offsets relativ zu den Basen; die Basisausrichtung selbst ist nicht belegt.
- Keine Verschiebungsoperation wird im C-Collision-Modell rekonstruiert; `ADD.W D4,D4` entspricht dem beobachteten Verdoppeln der Extent-Summe.

## Abweichungen

- Originale Eingaberegister A2/A3/A4/A6 und A5-Scratch-Speicher werden auf Hostpointer und sechs C-Ausgabeparameter abgebildet. Der genaue Parametervertrag ist UNKNOWN.
- C `bool` fasst D4=0/1 zusammen, aber bildet weder D4-Registerwert noch Condition Codes nach.
- Der Ghidra-Decompiler signiert beide Collision-Funktionsobjekte als `void`; der Aufrufer-/Registerkontext und der D4-Endwert begruenden die C-Adapterrueckgabe nur als PROBABLE.
- Boundspruefung und Vorzeichenkonversion lesen ungepruefte Descriptordaten. Die C-API hat keine Laengenparameter; gueltige Bytebereiche werden vom Hostcaller vorausgesetzt.
- Host-Bytezugriffe tolerieren ungerade Basen. Ein 68000-Wort-Memoryzugriff auf ungerade Adresse ist nicht aequivalent und kann eine CPU-Ausnahme ausloesen.

Es wurde keine Differenz in den 30 isolierten Testfaellen gegen die aus den beobachteten Instruktionen abgeleiteten Erwartungen gefunden. Das ist kein Beweis fuer alle Eingaben und kein dynamischer Vergleich mit dem originalen Code.
