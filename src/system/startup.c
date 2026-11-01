#include "../common/types.h"
#include "../memory/memory_map.h"

/*
 * CONFIRMED PREFIX of Ghidra FUN_0000040e.
 * Ghidra body ranges: 0x0000040e-0x00000425 and 0x0000073c-0x00000751.
 * The original finishes this block with JMP (A4). The Ghidra data reference
 * resolves the LEA at 0x0000041e to 0x00000426, where another LEA begins;
 * 0x00000428 is the extension word of that second LEA.
 * This C prefix deliberately stops before the jump and does not claim the rest
 * of the reset routine.
 * Do not execute this host-side representation against ordinary process memory.
 */
void recon_FUN_0000040e_prefix(void)
{
    volatile u8 *coin_control = (volatile u8 *)(uintptr_t)SF2_COIN_CONTROL_START;
    coin_control[0] = UINT8_C(0x80);
    coin_control[0] = UINT8_C(0x00);

    for (u32 address = SF2_GFX_RESET_CLEAR_START;
         address <= SF2_GFX_RESET_CLEAR_END;
         address += UINT32_C(2)) {
        volatile u16 *word = (volatile u16 *)(uintptr_t)address;
        *word = UINT16_C(0);
    }
}
