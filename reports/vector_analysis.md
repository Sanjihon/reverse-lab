# Motorola-68000-Exception-Vektoren

Untersucht wurde ausschließlich [`working/maincpu.bin`](../working/maincpu.bin) im Ghidra-Import `68000:BE:32:default`, Image Base `0x0`. MAMEs `main_map` verifiziert ROM ab `0x000000`; deshalb liegen die Vektoren am tatsächlichen CPU-Adressanfang. Ghidra lieferte für `0x0-0xff` 256 von 256 Bytes vollständig.

Jeder Vektoreintrag ist ein 32-Bit-Big-Endian-Wert; der Vektorindex `n` liegt bei Offset `4*n`. Die Tabelle deckt alle 64 Einträge (`0x00-0xff`) ab.

| Vektor | Offset | Zweck (68000) | gelesener Wert |
|---:|---:|---|---:|
| 0 | `0x00` | Initial Supervisor Stack Pointer (SSP) | `0x00000000` |
| 1 | `0x04` | Reset Program Counter (PC) | `0x0000040e` |
| 2 | `0x08` | Bus error | `0x00000400` |
| 3 | `0x0c` | Address error | `0x00000400` |
| 4 | `0x10` | Illegal instruction | `0x00000402` |
| 5 | `0x14` | Division by zero | `0x00000402` |
| 6 | `0x18` | CHK | `0x00000402` |
| 7 | `0x1c` | TRAPV | `0x00000402` |
| 8 | `0x20` | Privilege violation | `0x00000402` |
| 9 | `0x24` | Trace | `0x00000402` |
| 10 | `0x28` | Line-A emulator | `0x00000402` |
| 11 | `0x2c` | Line-F emulator | `0x00000402` |
| 12-23 | `0x30-0x5c` | Für 68000 reserviert; alle 12 Einträge | `0xffffffff` |
| 24 | `0x60` | Spurious interrupt | `0x0000040c` |
| 25 | `0x64` | Interrupt level 1 | `0x0000040c` |
| 26 | `0x68` | Interrupt level 2 | `0x00000a94` |
| 27 | `0x6c` | Interrupt level 3 | `0x0000040c` |
| 28 | `0x70` | Interrupt level 4 | `0x0000040c` |
| 29 | `0x74` | Interrupt level 5 | `0x0000040c` |
| 30 | `0x78` | Interrupt level 6 | `0x0000040c` |
| 31 | `0x7c` | Interrupt level 7 | `0x0000040c` |
| 32 | `0x80` | TRAP #0 | `0x00000b24` |
| 33 | `0x84` | TRAP #1 | `0x00000b8c` |
| 34 | `0x88` | TRAP #2 | `0x00000be4` |
| 35 | `0x8c` | TRAP #3 | `0x00000cc4` |
| 36 | `0x90` | TRAP #4 | `0x00000cf0` |
| 37 | `0x94` | TRAP #5 | `0x00000d18` |
| 38 | `0x98` | TRAP #6 | `0x00000d34` |
| 39 | `0x9c` | TRAP #7 | `0x00000b62` |
| 40 | `0xa0` | TRAP #8 | `0x00000c12` |
| 41 | `0xa4` | TRAP #9 | `0x00000bb0` |
| 42 | `0xa8` | TRAP #10 | `0x00000b46` |
| 43 | `0xac` | TRAP #11 | `0x00000c60` |
| 44 | `0xb0` | TRAP #12 | `0x00000ca2` |
| 45 | `0xb4` | TRAP #13 | `0x00000ca6` |
| 46 | `0xb8` | TRAP #14 | `0x00000402` |
| 47 | `0xbc` | TRAP #15 | `0x00000402` |
| 48-63 | `0xc0-0xfc` | Für 68000 reserviert; alle 16 Einträge | `0xffffffff` |

## Bewertung

Der Reset-PC `0x0000040e` ist gerade und liegt innerhalb des rekonstruierten ROM-Bereichs. Die übrigen gesetzten Vektorziele (`0x400`, `0x402`, `0x40c`, `0xa94` sowie die TRAP-Ziele `0xb24-0xd34`) sind ebenfalls gerade und liegen im gemappten 1-MiB-Image. Reservierte Slots enthalten den Sentinel `0xffffffff`, nicht einen im Image liegenden Handler.

Der Initial-SSP-Wert ist numerisch null. Das ist auffällig, aber nicht automatisch unplausibel: Ein 68000 besitzt einen 24-Bit-externen Adressbus; ein Stack-Predekrement von null kann an das obere 24-Bit-Adressende umbrechen. MAME mappt `0xff0000-0xffffff` als RAM. Das macht den Wert mit der Boardkarte vereinbar, beweist aber keine tatsächliche Laufzeit-Stackverwendung; statische Analyse emuliert den Reset oder Bus-Wrap nicht.