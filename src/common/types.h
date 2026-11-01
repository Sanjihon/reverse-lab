#ifndef SF2_RECON_COMMON_TYPES_H
#define SF2_RECON_COMMON_TYPES_H

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;

_Static_assert(sizeof(u8) == 1, "u8 must be 8 bits");
_Static_assert(sizeof(s8) == 1, "s8 must be 8 bits");
_Static_assert(sizeof(u16) == 2, "u16 must be 16 bits");
_Static_assert(sizeof(s16) == 2, "s16 must be 16 bits");
_Static_assert(sizeof(u32) == 4, "u32 must be 32 bits");
_Static_assert(sizeof(s32) == 4, "s32 must be 32 bits");
_Static_assert(CHAR_BIT == 8, "the reconstruction requires 8-bit bytes");

#endif
