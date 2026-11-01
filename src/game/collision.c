#include "../common/types.h"

/*
 * PROBABLE host-side model of the control flow rooted at Ghidra FUN_0007e486.
 * Ghidra has a 12-byte function object at 0x7e486 and a separate 156-byte
 * object at 0x7e492, although the former branches into the latter's code.
 * The original register ABI is not recovered. A2/A3/A4/A6 and A5-relative
 * scratch outputs are represented here as explicit byte pointers/output slots.
 * This is not original Capcom source and is not a hardware emulator routine.
 */
static u16 add16(u16 left, u16 right)
{
    return (u16)((u32)left + (u32)right);
}

static u16 sub16(u16 left, u16 right)
{
    return (u16)((u32)left - (u32)right);
}

static s16 s16_from_bits(u16 value)
{
    s32 signed_value = (value < UINT16_C(0x8000))
        ? (s32)value
        : (s32)value - INT32_C(0x10000);
    return (s16)signed_value;
}

static s16 sign_extend_byte(u8 value)
{
    s16 signed_value = (value < UINT8_C(0x80))
        ? (s16)value
        : (s16)((s16)value - INT16_C(0x100));
    return signed_value;
}

static s16 load_s16_be(const u8 *base, size_t offset)
{
    u16 bits = (u16)(((u16)base[offset] << 8) | (u16)base[offset + 1]);
    return s16_from_bits(bits);
}

static s16 negate_s16_wrap(s16 value)
{
    return s16_from_bits((u16)(UINT16_C(0) - (u16)value));
}

/*
 * Address mapping: combined entry at 0x7e486, shared comparison body at 0x7e492.
 * Returns the D4/condition-code success decision as a C bool model. The real
 * call site branches on Z after BSR; this is not a recovered source signature.
 */
bool recon_FUN_0007e486_overlap_model(
    const u8 *a2_object,
    const u8 *a3_descriptor,
    const u8 *a4_descriptor,
    const u8 *a6_object,
    s16 *scratch_x_a,
    s16 *scratch_x_b,
    s16 *scratch_y_a,
    s16 *scratch_y_b,
    s16 *delta_x_out,
    s16 *delta_y_out)
{
    s16 a3_x_offset;
    if ((a3_descriptor[5] & UINT8_C(0x80)) != 0) {
        a3_x_offset = s16_from_bits((u16)(UINT16_C(0) - (u16)a3_descriptor[5]));
    } else {
        a3_x_offset = sign_extend_byte(a3_descriptor[0]);
    }

    if (a6_object[0x12] != 0) {
        a3_x_offset = negate_s16_wrap(a3_x_offset);
    }
    s16 x_a = s16_from_bits(add16((u16)load_s16_be(a6_object, 6),
                                  (u16)a3_x_offset));
    s16 a4_x_offset = sign_extend_byte(a4_descriptor[0]);
    if (a2_object[0x12] != 0) {
        a4_x_offset = negate_s16_wrap(a4_x_offset);
    }
    s16 x_b = s16_from_bits(add16((u16)load_s16_be(a2_object, 6),
                                  (u16)a4_x_offset));
    *scratch_x_a = x_a;
    *scratch_x_b = x_b;

    u16 extent_x = add16((u16)sign_extend_byte(a3_descriptor[2]),
                         (u16)sign_extend_byte(a4_descriptor[2]));
    u16 delta_x = sub16((u16)x_a, (u16)x_b);
    if (add16(extent_x, delta_x) > add16(extent_x, extent_x)) {
        return false;
    }

    u16 adjusted_x = add16(delta_x,
        (s16_from_bits(delta_x) < 0) ? extent_x : (u16)(UINT16_C(0) - extent_x));
    *delta_x_out = s16_from_bits(adjusted_x);

    s16 y_a = s16_from_bits(add16((u16)load_s16_be(a6_object, 10),
                                  (u16)sign_extend_byte(a3_descriptor[1])));
    s16 y_b = s16_from_bits(add16((u16)load_s16_be(a2_object, 10),
                                  (u16)sign_extend_byte(a4_descriptor[1])));
    *scratch_y_a = y_a;
    *scratch_y_b = y_b;

    u16 extent_y = add16((u16)sign_extend_byte(a3_descriptor[3]),
                         (u16)sign_extend_byte(a4_descriptor[3]));
    u16 delta_y = sub16((u16)y_a, (u16)y_b);
    if (add16(extent_y, delta_y) > add16(extent_y, extent_y)) {
        return false;
    }

    u16 adjusted_y = add16(delta_y,
        (s16_from_bits(delta_y) < 0) ? extent_y : (u16)(UINT16_C(0) - extent_y));
    *delta_y_out = s16_from_bits(adjusted_y);
    return true;
}
