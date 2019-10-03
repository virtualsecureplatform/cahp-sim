#include "inst.h"
#include "bitpat.h"
#include "cpu.h"
#include "log.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint32_t get_bits(uint32_t t, int s, int e)
{
    int bit_len = e - s;
    uint32_t bit_mask = 1;
    for (int i = 0; i < bit_len; i++) {
        bit_mask = (bit_mask << 1) + 1;
    }
    return (t >> s) & bit_mask;
}

static uint16_t sign_ext(uint16_t t, uint8_t sign_bit)
{
    uint16_t sign_v = 0;
    uint16_t sign = get_bits(t, sign_bit, sign_bit);
    for (int i = sign_bit; i < 16; i++) {
        sign_v |= (sign << i);
    }
    return t | sign_v;
}

static const char *reg2str(int regno)
{
    switch (regno) {
    case 0: return "x0";
    case 1: return "x1";
    case 2: return "x2";
    case 3: return "x3";
    case 4: return "x4";
    case 5: return "x5";
    case 6: return "x6";
    case 7: return "x7";
    case 8: return "x8";
    case 9: return "x9";
    case 10: return "x10";
    case 11: return "x11";
    case 12: return "x12";
    case 13: return "x13";
    case 14: return "x14";
    case 15: return "x15";
    }

    assert(0 && "Invalid register index!");
}

#define DEFINE_INST24_RRR(inst_name, op, calc_expr)                       \
    static void inst_##inst_name(struct cpu *c, uint32_t inst)            \
    {                                                                     \
        uint8_t rd = get_bits(inst, 8, 11), rs1 = get_bits(inst, 12, 15), \
                rs2 = get_bits(inst, 16, 19);                             \
                                                                          \
        uint16_t lhs = reg_read(c, rs1), rhs = reg_read(c, rs2);          \
        uint32_t res = (calc_expr);                                       \
        reg_write(c, rd, res & 0xFFFF);                                   \
                                                                          \
        pc_update(c, 3);                                                  \
                                                                          \
        log_printf(#inst_name " %s, %s, %s\n", reg2str(rd), reg2str(rs1), \
                   reg2str(rs2));                                         \
        log_printf("\t%04x = %04x " op " %04x\n", res, lhs, rhs);         \
        log_printf("\t%s <= %04x\n", reg2str(rd), res);                   \
        log_printf("\tPC <= %04x\n", pc_read(c));                         \
    }
#include "inst24.inc"
#undef DEFINE_INST24_RRR

#define DEFINE_INST16_RR(inst_name, op, calc_expr)                       \
    static void inst_##inst_name##2(struct cpu * c, uint16_t inst)       \
    {                                                                    \
        uint8_t rd = get_bits(inst, 8, 11), rs = get_bits(inst, 12, 15); \
                                                                         \
        uint16_t lhs = reg_read(c, rd);                                  \
        uint16_t rhs = reg_read(c, rs);                                  \
        uint32_t res = (calc_expr);                                      \
        reg_write(c, rd, res & 0xFFFF);                                  \
                                                                         \
        pc_update(c, 2);                                                 \
                                                                         \
        log_printf(#inst_name "2 %s, %s\n", reg2str(rd), reg2str(rs));   \
        log_printf("\t%04x = %04x " op " %04x\n", res, lhs, rhs);        \
        log_printf("\t%s <= %04x\n", reg2str(rd), res);                  \
        log_printf("\tPC <= %04x\n", pc_read(c));                        \
    }
#include "inst16.inc"
#undef DEFINE_INST16_RR

const struct inst24_info inst_list_24[] = {
    {"xxxx_xxxx_xxxx_xxxx_xx00_0001", inst_add},  // ADD
    {"xxxx_xxxx_xxxx_xxxx_xx00_1001", inst_sub},  // SUB
    {"xxxx_xxxx_xxxx_xxxx_xx01_0001", inst_and},  // AND
    {"xxxx_xxxx_xxxx_xxxx_xx01_1001", inst_xor},  // XOR
    {"xxxx_xxxx_xxxx_xxxx_xx10_0001", inst_or},   // OR
    {"xxxx_xxxx_xxxx_xxxx_xx10_1001", inst_lsl},  // LSL
    {"xxxx_xxxx_xxxx_xxxx_xx11_0001", inst_lsr},  // LSR
    {"xxxx_xxxx_xxxx_xxxx_xx11_1001", inst_asr},  // LSR
    {NULL, NULL}                                  // Terminator
};

const struct inst16_info inst_list_16[] = {
    {"xxxx_xxxx_1000_0000", inst_add2},  // ADD2
    {"xxxx_xxxx_1000_1000", inst_sub2},  // SUB2
    {"xxxx_xxxx_1001_0000", inst_and2},  // AND2
    {"xxxx_xxxx_1001_1000", inst_xor2},  // XOR2
    {"xxxx_xxxx_1010_0000", inst_or2},   // OR2
    {"xxxx_xxxx_1010_1000", inst_lsl2},  // LSL2
    {"xxxx_xxxx_1011_0000", inst_lsr2},  // LSR2
    {"xxxx_xxxx_1011_1000", inst_asr2},  // ASR2
    {NULL, NULL}                         // Terminator
};
