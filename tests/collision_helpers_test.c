#include <assert.h>
#include <stdint.h>

#include "../src/game/collision.c"

int main(void)
{
    const u8 encoded_words[] = {
        0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0xff, 0xff
    };

    assert(add16(UINT16_MAX, UINT16_C(1)) == UINT16_C(0));
    assert(add16(UINT16_C(0x7fff), UINT16_C(1)) == UINT16_C(0x8000));
    assert(sub16(UINT16_C(0), UINT16_C(1)) == UINT16_MAX);
    assert(s16_from_bits(UINT16_C(0x7fff)) == INT16_MAX);
    assert(s16_from_bits(UINT16_C(0x8000)) == INT16_MIN);
    assert(s16_from_bits(UINT16_C(0xffff)) == -1);
    assert(sign_extend_byte(UINT8_C(0x7f)) == INT8_MAX);
    assert(sign_extend_byte(UINT8_C(0x80)) == INT8_MIN);
    assert(sign_extend_byte(UINT8_C(0xff)) == -1);
    assert(load_s16_be(encoded_words, 0) == 0);
    assert(load_s16_be(encoded_words, 2) == INT16_MAX);
    assert(load_s16_be(encoded_words, 4) == INT16_MIN);
    assert(load_s16_be(encoded_words, 6) == -1);
    assert(negate_s16_wrap(INT16_C(1)) == -1);
    assert(negate_s16_wrap(INT16_MIN) == INT16_MIN);

    u8 object_a[0x14] = {0};
    u8 object_b[0x14] = {0};
    u8 descriptor_a[6] = {0, 0, 5, 4, 0, 0};
    u8 descriptor_b[6] = {0, 0, 5, 4, 0, 0};
    s16 scratch_x_a = 0;
    s16 scratch_x_b = 0;
    s16 scratch_y_a = 0;
    s16 scratch_y_b = 0;
    s16 delta_x = 123;
    s16 delta_y = 456;

    object_a[6] = 0x00;
    object_a[7] = 100;
    object_a[10] = 0x00;
    object_a[11] = 50;
    object_b[6] = 0x00;
    object_b[7] = 110;
    object_b[10] = 0x00;
    object_b[11] = 58;
    assert(recon_FUN_0007e486_overlap_model(
        object_b, descriptor_a, descriptor_b, object_a,
        &scratch_x_a, &scratch_x_b, &scratch_y_a, &scratch_y_b,
        &delta_x, &delta_y));
    assert(scratch_x_a == 100 && scratch_x_b == 110);
    assert(scratch_y_a == 50 && scratch_y_b == 58);
    assert(delta_x == 0 && delta_y == 0);

    object_b[7] = 111;
    delta_x = 123;
    delta_y = 456;
    assert(!recon_FUN_0007e486_overlap_model(
        object_b, descriptor_a, descriptor_b, object_a,
        &scratch_x_a, &scratch_x_b, &scratch_y_a, &scratch_y_b,
        &delta_x, &delta_y));
    assert(scratch_x_a == 100 && scratch_x_b == 111);
    assert(delta_x == 123 && delta_y == 456);

    object_b[7] = 110;
    object_b[11] = 59;
    delta_x = 123;
    delta_y = 456;
    assert(!recon_FUN_0007e486_overlap_model(
        object_b, descriptor_a, descriptor_b, object_a,
        &scratch_x_a, &scratch_x_b, &scratch_y_a, &scratch_y_b,
        &delta_x, &delta_y));
    assert(delta_x == 0 && delta_y == 456);

    object_b[11] = 50;
    object_a[0x12] = 1;
    object_b[0x12] = 1;
    descriptor_a[0] = 3;
    descriptor_a[2] = 7;
    descriptor_b[0] = 2;
    descriptor_b[2] = 7;
    assert(recon_FUN_0007e486_overlap_model(
        object_b, descriptor_a, descriptor_b, object_a,
        &scratch_x_a, &scratch_x_b, &scratch_y_a, &scratch_y_b,
        &delta_x, &delta_y));
    assert(scratch_x_a == 97 && scratch_x_b == 108);
    assert(delta_x == 3);

    object_a[6] = 0x00;
    object_a[7] = 100;
    object_b[6] = 0xff;
    object_b[7] = 0x67;
    object_a[0x12] = 0;
    object_b[0x12] = 0;
    descriptor_a[0] = 127;
    descriptor_a[2] = 127;
    descriptor_a[5] = 0xfe;
    descriptor_b[0] = 0;
    descriptor_b[2] = 127;
    assert(recon_FUN_0007e486_overlap_model(
        object_b, descriptor_a, descriptor_b, object_a,
        &scratch_x_a, &scratch_x_b, &scratch_y_a, &scratch_y_b,
        &delta_x, &delta_y));
    assert(scratch_x_a == -154 && scratch_x_b == -153);
    assert(delta_x == 253);

    return 0;
}