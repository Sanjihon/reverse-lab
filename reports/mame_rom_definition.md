# MAME-ROM-Definition: Street Fighter II `sf2eb`

## Identifikation und Quelle

Die identifizierte Set-Bezeichnung ist **`sf2eb`**, „Street Fighter II: The World Warrior (World 910214)“. MAME registriert das Set als 1991er CPS-1-Spiel. Die Phase-1-CRC-Prüfung ordnet alle acht lokalen Programmdateien exakt den acht Programm-ROMs in dieser Definition zu.

Primärquelle: [MAME `src/mame/capcom/cps1.cpp`, `ROM_START(sf2eb)`](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L7199) und [Spielregistrierung](https://github.com/mamedev/mame/blob/master/src/mame/capcom/cps1.cpp#L15026). `CODE_SIZE` ist in derselben Datei als `0x400000` definiert. Quelle geprüft am 2026-10-31.

## Vollständige ROM-Definition

Die folgende Wiedergabe enthält sämtliche ROM-Regions- und ROM-Ladeanweisungen des Sets, einschließlich CRC/SHA1 und aller `CONTINUE`-Anweisungen. Es gibt in diesem Block keine `ROM_RELOAD`-Anweisung.

```cpp
ROM_START( sf2eb )
        ROM_REGION( CODE_SIZE, "maincpu", 0 )      /* 68000 code */
        ROM_LOAD16_BYTE( "sf2e_30b.11e", 0x00000, 0x20000, CRC(57bd7051) SHA1(5e211e75b1649b07723cabc03cf15636dbbae595) )
        ROM_LOAD16_BYTE( "sf2e_37b.11f", 0x00001, 0x20000, CRC(62691cdd) SHA1(328703c3e737ada544e67c36119eeb4a100ca740) )   // only rom different from sf2ub
        ROM_LOAD16_BYTE( "sf2e_31b.12e", 0x40000, 0x20000, CRC(a673143d) SHA1(e565f0ec23d6deb543c72af5a83f070c07319477) )
        ROM_LOAD16_BYTE( "sf2e_38b.12f", 0x40001, 0x20000, CRC(4c2ccef7) SHA1(77b119c70c255622b023de25d9af3b3aac52ea47) )
        ROM_LOAD16_BYTE( "sf2_28b.9e",   0x80000, 0x20000, CRC(4009955e) SHA1(7842dbef7650485639fbae49b9f4db7494d4f73d) )
        ROM_LOAD16_BYTE( "sf2_35b.9f",   0x80001, 0x20000, CRC(8c1f3994) SHA1(5e1d334399d05a837c2d80f79eada543e83afaf7) )
        ROM_LOAD16_BYTE( "sf2_29b.10e",  0xc0000, 0x20000, CRC(bb4af315) SHA1(75f0827f4f7e9f292add46467f8d4fe19b2514c9) )
        ROM_LOAD16_BYTE( "sf2_36b.10f",  0xc0001, 0x20000, CRC(c02a13eb) SHA1(b807cc495bff3f95d03b061fc629c95f965cb6d8) )

        ROM_REGION( 0x600000, "gfx", 0 )
        ROM_LOAD64_WORD( "sf2-5m.4a",  0x000000, 0x80000, CRC(22c9cc8e) SHA1(b9194fb337b30502c1c9501cd6c64ae4035544d4) )    // in "2" socket
        ROM_LOAD64_WORD( "sf2-7m.6a",  0x000002, 0x80000, CRC(57213be8) SHA1(3759b851ac0904ec79cbb67a2264d384b6f2f9f9) )    // in "4" socket
        ROM_LOAD64_WORD( "sf2-1m.3a",  0x000004, 0x80000, CRC(ba529b4f) SHA1(520840d727161cf09ca784919fa37bc9b54cc3ce) )    // in "1" socket
        ROM_LOAD64_WORD( "sf2-3m.5a",  0x000006, 0x80000, CRC(4b1b33a8) SHA1(2360cff890551f76775739e2d6563858bff80e41) )    // in "3" socket
        ROM_LOAD64_WORD( "sf2-6m.4c",  0x200000, 0x80000, CRC(2c7e2229) SHA1(357c2275af9133fd0bd6fbb1fa9ad5e0b490b3a2) )    // in "11" socket
        ROM_LOAD64_WORD( "sf2-8m.6c",  0x200002, 0x80000, CRC(b5548f17) SHA1(baa92b91cf616bc9e2a8a66adc777ffbf962a51b) )    // in "13" socket
        ROM_LOAD64_WORD( "sf2-2m.3c",  0x200004, 0x80000, CRC(14b84312) SHA1(2eea16673e60ba7a10bd4d8f6c217bb2441a5b0e) )    // in "10" socket
        ROM_LOAD64_WORD( "sf2-4m.5c",  0x200006, 0x80000, CRC(5e9cd89a) SHA1(f787aab98668d4c2c54fc4ba677c0cb808e4f31e) )    // in "12" socket
        ROM_LOAD64_WORD( "sf2-13m.4d", 0x400000, 0x80000, CRC(994bfa58) SHA1(5669b845f624b10e7be56bfc89b76592258ce48b) )    // in "21" socket
        ROM_LOAD64_WORD( "sf2-15m.6d", 0x400002, 0x80000, CRC(3e66ad9d) SHA1(9af9df0826988872662753e9717c48d46f2974b0) )    // in "23" socket
        ROM_LOAD64_WORD( "sf2-9m.3d",  0x400004, 0x80000, CRC(c1befaa8) SHA1(a6a7f4725e52678cbd8d557285c01cdccb2c2602) )    // in "20" socket
        ROM_LOAD64_WORD( "sf2-11m.5d", 0x400006, 0x80000, CRC(0627c831) SHA1(f9a92d614e8877d648449de2612fc8b43c85e4c2) )    // in "22" socket

        ROM_REGION( 0x18000, "audiocpu", 0 ) /* 64k for the audio CPU (+banks) */
        ROM_LOAD( "sf2_9.12a", 0x00000, 0x08000, CRC(a4823a1b) SHA1(7b6bf59dfd578bfbbdb64c27988796783442d659) )
        ROM_CONTINUE(           0x10000, 0x08000 )

        ROM_REGION( 0x40000, "oki", 0 ) /* Samples */
        ROM_LOAD( "sf2_18.11c", 0x00000, 0x20000, CRC(7f162009) SHA1(346bf42992b4c36c593e21901e22c87ae4a7d86d) )
        ROM_LOAD( "sf2_19.12c", 0x20000, 0x20000, CRC(beade53f) SHA1(277c397dc12752719ec6b47d2224750bd1c07f79) )

        ROM_REGION( 0x0200, "aboardplds", 0 )
        ROM_LOAD( "buf1", 0x0000, 0x0117, CRC(eb122de7) SHA1(b26b5bfe258e3e184f069719f9fd008d6b8f6b9b) )
        ROM_LOAD( "ioa1", 0x0000, 0x0117, CRC(59c7ee3b) SHA1(fbb887c5b4f5cb8df77cec710eaac2985bc482a6) )
        ROM_LOAD( "prg1", 0x0000, 0x0117, CRC(f1129744) SHA1(a5300f301c1a08a7da768f0773fa0fe3f683b237) )
        ROM_LOAD( "rom1", 0x0000, 0x0117, CRC(41dc73b9) SHA1(7d4c9f1693c821fbf84e32dd6ef62ddf14967845) )
        ROM_LOAD( "sou1", 0x0000, 0x0117, CRC(84f4b2fe) SHA1(dcc9e86cc36316fe42eace02d6df75d08bc8bb6d) )

        ROM_REGION( 0x0200, "bboardplds", 0 )
        ROM_LOAD( "stf29.1a", 0x0000, 0x0117, CRC(043309c5) SHA1(f6b9610c9f896c495041ba56a654776a521c356b) )
        ROM_LOAD( "iob1.11d", 0x0000, 0x0117, CRC(3abc0700) SHA1(973043aa46ec6d5d1db20dc9d5937005a0f9f6ae) )

        ROM_REGION( 0x0200, "cboardplds", 0 )
        ROM_LOAD( "c632.ic1", 0x0000, 0x0117, CRC(0fbd9270) SHA1(d7e737b20c44d41e29ca94be56114b31934dde81) )
ROM_END
```

## Einordnung

- Alle acht Programm-ROM-Loads sind `ROM_LOAD16_BYTE` mit je `0x20000` Bytes. Die vier Offsetpaare und ihre lokalen CRC-Treffer sind in [rom_pair_mapping.csv](rom_pair_mapping.csv) detailliert aufgeführt.
- Die geraden Offsets laden auf die gerade Byte-Lane, die ungeraden Offsets auf die ungerade Lane. Für den 68000 mit 16-Bit-Bus liegt das Byte an gerader Adresse im höherwertigen Byte des Big-Endian-Worts.
- Nur der Sound-CPU-ROM verwendet `ROM_CONTINUE`: die ersten `0x8000` Quelldatenbytes liegen bei `0x00000`, die nächsten `0x8000` bei `0x10000`. Die Region lässt dadurch `0x08000` bis `0x0ffff` frei.
- Die acht in Phase 1 als fehlend ausgewiesenen PLD-Dateien sind in der MAME-Definition enthalten; sie gehören nicht zu den acht Programm-ROMs.