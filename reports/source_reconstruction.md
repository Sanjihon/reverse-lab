# Phase 9: Strukturierte C-Rekonstruktion

## Zweck und Abgrenzung

Diese Dateien sind eine nachvollziehbare, partielle C-Rekonstruktion aus dem verifizierten ROM `working/maincpu.bin`, MAME `sf2eb` und dem bestehenden Ghidra-Projekt `working/SF2MainCPU.gpr`. Sie sind **nicht** der originale Capcom-Quellcode und keine lauffaehige Arcade-Portierung. Nicht verstandene Funktionen wurden nicht mit erfundenem Verhalten implementiert.

Die Eingabe-ROMs in `sf2/` und das rekonstruierte Image `working/maincpu.bin` wurden nicht veraendert. Der Ghidra-Import wurde mit `-noanalysis` nur lesend fuer Evidenzabfragen verwendet.

## Angelegte Source-Dateien

```text
src/
  common/types.h          fixed-width u8/s8/u16/s16/u32/s32; Groessenassertions
  memory/memory_map.h     verifizierte MAME-Adressen und Portmasken
  system/startup.c        CONFIRMED-Teilmodell der Reset-Seiteneffekte
  system/interrupts.c     gelesene Vektorindex-/Zielwerttabelle, keine Handlerimplementation
  game/collision.c        PROBABLE Host-Modell des Overlap-Pfads plus 6 arithmetische Helfer
```

Die vorgeschlagene Projektstruktur wurde bewusst nicht vollstaendig erzeugt: Es gibt keinen Beleg fuer eine C-`main()`, Game Loop, Player-/Character-Struktur, Input-Leseroutine, Animation-API, Damage-/HP-Variable oder Spezialbewegung. Daher wurden keine `main.c`, `player.c`, `game_loop.c`, `damage.c` oder `characters/`-Implementierungen angelegt.

## Ghidra-zu-C-Zuordnung

| C-Funktion | Ghidra-Adresse(n) | Status | Rekonstruktionsumfang |
|---|---|---|---|
| `recon_FUN_0000040e_prefix` | `0x0000040e` und disjunkter Ghidra-Block `0x0000073c` | **CONFIRMED**, partiell | Belegte Writes an `0x800030` und Nullwort-Clear `0x900000-0x903fff`; kein Modell der statisch auf `0x426` rueckfuehrbaren `JMP (A4)`-Folge oder der weiteren Initialisierung. |
| `recon_FUN_0007e486_overlap_model` | `0x00007e486` (12-Byte-Entryfragment) + `0x00007e492` (156-Byte-Comparator) | **PROBABLE** | Host-C-Abbildung des Byteoffset-/Facing-/2D-Extentvergleichs. Die beiden Ghidra-Starts sind getrennte Symbole; der C-Eintrag fuehrt sie fuer die beobachtete gemeinsame Kontrollflussstrecke zusammen. Host-Parameter und `bool`-Rueckgabe sind Adapter, nicht wiederhergestellte 68000-ABI. |

Die vollstaendige Mappingtabelle fuer alle 353 Ghidra-Funktionen ist [source_function_mapping.csv](source_function_mapping.csv). Sie wurde per Ghidra-API direkt aus dem gespeicherten Projekt exportiert; Ghidra-Namen wurden nicht veraendert.

## CONFIRMED: Reset-Seiteneffekte

Ghidra belegt am Reset-PC `0x40e` zwei Bytewrites an CPS Coin-Control `0x800030`, eine LEA bei `0x41e` mit DATA-Xref auf `0x426` und `BRA.W 0x73c`. Bei `0x73c` werden A0=`0x900000`, A1=`0x903fff` gesetzt und Nullworte bis zum Ende des Bereichs geschrieben. Der statische Registerdatenfluss von `JMP (A4)` bei `0x750` fuehrt zu `0x426`; `0x428` ist das Extension-Word der zweiten LEA. Die C-Funktion bildet nur die Seiteneffekte ab und behauptet weder vollstaendigen Reset-Control-Flow noch Laufzeitgleichheit.

Die Ghidra-Funktion `FUN_0000040e` hat zwei getrennte Ranges (`0x40e-0x425` und `0x73c-0x751`). Die Adresse `0x426` ist eine separate LEA; `0x428` ist deren Extension-Word und selbst kein Instruktionsstart. Diese Trennung ist im Ghidra-Evidence-Log dokumentiert.

## PROBABLE: Overlap-Modell

`collision.c` modelliert die Instruktionen von `0x7e486` und `0x7e492` als einen Host-Aufruf: Byteoffsetwahl an A3, Vorzeichenextension, Facing-Spiegelung, 16-Bit-wrap-faehige Koordinatensummen, Extent-Summen und zwei unsigned Boundsvergleiche. Der Aufrufer testet nach `BSR` das Zero-Flag; im C-Modell wird die Erfolg/Fehlschlag-Entscheidung als `bool` zurueckgegeben. Das ist eine explizite Schnittstellenannahme, nicht die originale Register-/Flagsignatur.

Die konkreten Descriptor-/Objektzeiger werden als Bytepointer uebergeben, weil ein struct-Layout nicht belegt ist. Keine Hitbox/Hurtbox-Klassifikation, kein Schaden und kein Knockback sind implementiert.

## Hilfsfunktionen und Daten

`collision.c` enthaelt sechs lokale Hilfsfunktionen fuer 16-Bit-Wrap-Arithmetik, Bit-zu-signed-Konvertierung, Byte-Signextension, Big-Endian-`s16`-Load und 68000-artige Negation. Diese sind C-Unterstuetzung, keine zusaetzlichen rekonstruierten Ghidra-Funktionen.

`interrupts.c` speichert 36 beobachtete nichtreservierte Vektorindex-/Ziel-Paare als Daten. Das ist keine Implementation der Exception-/Interrupt-Handler. `sf2_verified_vector` ist ein logischer Host-Datensatz; C-Struct-Padding wird nicht als Layout der Originalvektortabelle ausgegeben.

## Build-Vorbereitung

Es gibt keinen Build fuer ein Spiel und keine Emulatorabhaengigkeit. Nur Syntax und Typgroessen wurden mit dem Host-Compiler geprueft; es wurde nichts gelinkt oder als Arcade-Programm ausgefuehrt. Details und gemessene Resultate: [source_quality_report.md](source_quality_report.md).