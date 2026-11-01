#include "../common/types.h"

/*
 * Numeric 68000 vector values read big-endian from maincpu.bin at 0x000000-0x0000ff.
 * Reserved entries are omitted; each row retains its original vector index.
 * These are data observations, not reconstructed interrupt implementations.
 */
typedef struct {
    u16 vector_index;
    u32 target_address;
} sf2_verified_vector;

const sf2_verified_vector sf2_verified_vectors[] = {
    {0, 0x00000000}, {1, 0x0000040e}, {2, 0x00000400}, {3, 0x00000400},
    {4, 0x00000402}, {5, 0x00000402}, {6, 0x00000402}, {7, 0x00000402},
    {8, 0x00000402}, {9, 0x00000402}, {10, 0x00000402}, {11, 0x00000402},
    {24, 0x0000040c}, {25, 0x0000040c}, {26, 0x00000a94}, {27, 0x0000040c},
    {28, 0x0000040c}, {29, 0x0000040c}, {30, 0x0000040c}, {31, 0x0000040c},
    {32, 0x00000b24}, {33, 0x00000b8c}, {34, 0x00000be4}, {35, 0x00000cc4},
    {36, 0x00000cf0}, {37, 0x00000d18}, {38, 0x00000d34}, {39, 0x00000b62},
    {40, 0x00000c12}, {41, 0x00000bb0}, {42, 0x00000b46}, {43, 0x00000c60},
    {44, 0x00000ca2}, {45, 0x00000ca6}, {46, 0x00000402}, {47, 0x00000402}
};

const size_t sf2_verified_vector_count =
    sizeof(sf2_verified_vectors) / sizeof(sf2_verified_vectors[0]);
