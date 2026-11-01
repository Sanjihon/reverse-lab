# SF2-Startup-Analyse

Grundlage ist Ghidra 12.1.4 mit `68000:BE:32:default`, Compiler Spec `default` und Image Base `0x0`. Der Reset-PC stammt aus dem Big-Endian-Vektor 1 bei Offset `0x04` und lautet `0x0000040e`. Ghidras Postscript verifizierte Sprache und Basis, disassemblierte diesen PC und legte eine Funktion am Reset-PC an.

## Vom Reset-PC dekodierte Instruktionen

| Adresse | Bytes | Ghidra-Disassembly |
|---:|---|---|
| `0x0000040e` | `13fc008000800030` | `move.b #-0x80, (0x00800030).l` |
| `0x00000416` | `13fc000000800030` | `move.b #0x0, (0x00800030).l` |
| `0x0000041e` | `49fa0006` | `lea (0x6,PC), A4` |
| `0x00000422` | `60000318` | `bra.w 0x0000073c` |

Der Ghidra-Listinglauf meldete an `0x426` `<not decoded>`; das ist der lineare Fall-through nach einem unbedingten `BRA.W`, nicht die gewählte nächste Instruktion. Der Branch-Zielwert `0x73c` ist gerade und liegt im importierten ROM-Bereich. Die Bytes an `0x40e` wurden vollständig aus Ghidras Speicher gelesen; die Instruktionsmnemonics stammen aus Ghidras tatsächlichem `68000:BE:32:default`-Decoder.

## Plausibilität und Stack

Die ersten beiden `MOVE.B`-Instruktionen schreiben an `0x00800030`. MAME bildet `0x800030-0x800037` als CPS-1-Coin-Control-Register ab; die Zugriffe sind daher für dieses Board plausibel. Danach folgen ein PC-relatives `LEA` und ein gerader, innerhalb des Images liegender 16-Bit-`BRA`-Sprung. Opcode-Längen, Big-Endian-Wortfolge, gerade Instruktionsadressen und Branch-Ziel sind konsistent mit einem 68000-Resetpfad.

Der initiale SSP aus dem Vektor ist `0x00000000`. MAME weist `0xff0000-0xffffff` RAM zu. In Verbindung mit dem 24-Bit-Adressbus ist ein Predekrement-Stack am oberen Adressende möglich; in den vier hier ausgegebenen Startinstruktionen wird der Stack noch nicht verwendet. Ob und wann der folgende Code den Stack nutzt oder initialisiert, wurde nicht weiter verfolgt.

## Analysegrenzen

Ghidras Standard-Autoanalyse meldete Erfolg, erzeugte aber 488 nicht-fatale PCode-Decompiler-Warnungen über ungültige/nicht ausgerichtete Instruktionsadressen oder nicht auflösbare Konstrukte. Der Reset-PC war vor dem Postscript keine definierte Ghidra-Funktion; die Funktion wurde deshalb nach Prüfung von Language-ID und Basis explizit am aus dem Vektor gelesenen PC angelegt. Es erfolgte keine Emulation, keine Laufzeitanalyse und kein weiteres Tracing über den Reset-Branch hinaus.