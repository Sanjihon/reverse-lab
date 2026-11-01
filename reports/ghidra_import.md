# Ghidra-Import: SF2-Programmabbild

Stand: 2026-10-31. Importziel ist ausschließlich [`working/maincpu.bin`](../working/maincpu.bin), SHA-256 `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`.

## Verifizierte Basis

MAME registriert `sf2eb` mit Maschinenprofil `cps1_10MHz` ([Registrierung](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L15026)). Dieses Profil bindet die 68000-Programmadresskarte `main_map` ([CPU-Konfiguration](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L3909-L3918)). `main_map` ordnet ROM dem CPU-Bereich `0x000000-0x3fffff` zu und RAM `0xff0000-0xffffff` ([Adresskarte](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L577-L599)). Die Set-Definition lädt die rekonstruierten Programmbytes ab Regionoffset `0x000000`; daher ist die verifizierte Raw-Binary-Basis `0x000000`, nicht eine geschätzte Adresse.

Das Image ist `0x100000` Byte groß. Ghidra bildet daher tatsächlich `0x000000-0x0fffff` ab; es wurde kein künstlicher Speicher für den Rest der 4-MiB-MAME-ROM-Region angehängt.

## Installation und Importparameter

Ghidra 12.1.4 wurde sowohl durch REA-Provider-Evidence als auch durch die lokale Installation `/home/user/tools/ghidra/ghidra_12.1.4_PUBLIC` bestätigt. Die installierte Prozessorfamilie enthält `m68k`; die ausgewählte Language-ID ist exakt `68000:BE:32:default`.

| Parameter | Verwendeter Wert |
|---|---|
| Format / Loader | Raw Binary / `BinaryLoader` |
| Prozessorfamilie | `m68k` (Motorola 68000) |
| Ghidra Language-ID | `68000:BE:32:default` |
| Endianness | Big Endian, durch Language-ID |
| Compiler Specification | `default` |
| Image Base / Loader-Basis | `0x00000000` |
| Eingabedatei | `/home/user/reverse-lab/working/maincpu.bin` |
| Separater Executable-Header-Entry-Point | keiner; Raw-Datei ohne Header. Reset-PC wird aus dem Vektor bei Offset 4 gelesen: `0x0000040e` |
| Analyse | Ghidra-Standard-Autoanalyse nach erfolgreicher Auswahl von Language und Basis |
| Projektname / Speicherort | `SF2MainCPU`, `/home/user/reverse-lab/working` |
| Projektdateien | `working/SF2MainCPU.gpr` und `working/SF2MainCPU.rep/` |

Ausgeführter Importbefehl:

```sh
/home/user/tools/ghidra/ghidra_12.1.4_PUBLIC/support/analyzeHeadless /home/user/reverse-lab/working SF2MainCPU -import /home/user/reverse-lab/working/maincpu.bin -processor 68000:BE:32:default -cspec default -loader BinaryLoader -loader-baseAddr 0x0 -scriptPath /home/user/reverse-lab/tools -postScript InspectSf2Startup.java
```

`tools/InspectSf2Startup.java` prüft Language-ID und Image Base zur Laufzeit, disassembliert den Reset-PC und legt dort eine Funktion an. Der Headless-Log meldet `Analysis succeeded`, `Post-analysis succeeded`, `Save succeeded` und `Import succeeded`; das Projekt wurde persistent unter den oben genannten Pfaden gespeichert.

## Autoanalyse und Grenzen

Die Autoanalyse lief erst nach Festlegung von `68000:BE:32:default` und `0x0`. Der Import-Log enthält 488 nicht-fatale `WARN Decompiling`-PCode-Warnungen (u. a. ungültige, nicht ausgerichtete Instruktionsadressen/unerwartete Konstrukte). Der Import und die Autoanalyse meldeten dennoch Erfolg. Diese Warnungen sind ein Grund, globale automatische Funktionsgrenzen und Decompiler-Ausgaben nicht ungeprüft als Programmfluss zu behandeln.

Die ausgewählte Ghidra-Language-ID ist Ghidras 68000-Big-Endian-Default und dekodiert laut Provider-Evidence den 68040-Superset. Die Rohdatei bleibt unverändert. REA hatte zusätzlich einen ephemeren, schreibgeschützten Analysekontext geöffnet; dieser wird beim Schließen verworfen. Das persistente Headless-Projekt ist davon unabhängig.

## Verifizierte Datei- und Speicheridentität

- Eingabegröße: `0x100000` Byte; Ghidra-Image-Base: `0x0`.
- Ghidra-Segment im REA-Import: `ram`, `0x0-0x100000` (Endadresse exklusiv), Länge `0x100000`.
- Eingabe-SHA-256 stimmt mit Phase 3 überein: `6287819ca76c1a06e65499bd92196b29cf13d5ca1aab71c4bf90fb17113e72fd`.
- Vektor- und Reset-Codebytes wurden aus genau diesem Image gelesen; siehe [vector_analysis.md](vector_analysis.md) und [startup_analysis.md](startup_analysis.md).