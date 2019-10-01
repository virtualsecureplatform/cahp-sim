#include "inst.h"
#include "bitpat.h"
#include "cpu.h"
#include "log.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint16_t get_bits(uint16_t t, int s, int e)
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
    case 0: return "ra";
    case 1: return "sp";
    case 2: return "fp";
    case 3: return "s0";
    case 4: return "s1";
    case 5: return "s2";
    case 6: return "s3";
    case 7: return "s4";
    case 8: return "a0";
    case 9: return "a1";
    case 10: return "a2";
    case 11: return "a3";
    case 12: return "a4";
    case 13: return "a5";
    case 14: return "t0";
    case 15: return "t1";
    }

    assert(0 && "Invalid register index!");
}

static void inst_add(struct cpu *c, uint16_t inst)
{
    uint8_t rd = get_bits(inst, 8, 11), rs1 = get_bits(inst, 12, 15),
            rs2 = get_bits(inst, 16, 19);

    uint16_t lhs = reg_read(c, rs1), rhs = reg_read(c, rs2);
    uint32_t res = lhs + rhs;
    reg_write(c, rd, res & 0xFFFF);

    pc_update(c, 3);

    log_printf("add %s, %s, %s\n", reg2str(rd), reg2str(rs1), reg2str(rs2));
    log_printf("\t%04x = %04x + %04x\n", res, lhs, rhs);
    log_printf("\t  PC = %04x\n", pc_read(c));
}

static void inst_add2(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:ADD2\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    uint16_t d_data = reg_read(c, rd);
    uint32_t res = s_data + d_data;
    reg_write(c, rd, res & 0xFFFF);
    pc_update(c, 2);
}

const struct inst_data inst_list_24[] = {
    {"xxxx_xxxx_xxxx_xxxx_xx00_0001", inst_add},  // ADD
    {NULL, NULL}                                  // Terminator
};

const struct inst_data inst_list_16[] = {
    {"xxxx_xxxx_1000_0000", inst_add2},  // ADD2
    {NULL, NULL}                         // Terminator
};
