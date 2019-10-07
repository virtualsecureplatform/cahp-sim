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

static uint16_t sext(uint8_t nbits, uint16_t t)
{
    uint8_t sign_bit = nbits - 1;
    return t | (((t >> sign_bit) & 1) == 0 ? /* positive */ 0
                                           : /* negative */ ~0 << nbits);
}

static uint16_t asr(uint16_t lhs, uint16_t rhs)
{
    assert(rhs <= 16 && "Too large rhs");

    // NOTE: According to N1548, the value of (((int32_t)lhs) >> rhs) is
    // implementation-defined if lhs is negative.
    return (lhs >> 15) == 0 ? /* positive */ lhs >> rhs
                            : /* negative */ (~0 << (16 - rhs)) | (lhs >> rhs);
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

// src0 = src1 op src2
#define DEFINE_INST24_ARITH(inst_name, op, src0_expr, src1_expr, src2_expr,  \
                            lhs_expr, rhs_expr, calc_expr, logfmt, src0str,  \
                            src1str, src2str)                                \
    static void inst_##inst_name(struct cpu *c, uint32_t inst)               \
    {                                                                        \
        uint16_t src0 = (src0_expr), src1 = (src1_expr), src2 = (src2_expr); \
        uint16_t lhs = (lhs_expr), rhs = (rhs_expr);                         \
        uint16_t res = (calc_expr);                                          \
                                                                             \
        reg_write(c, src0, res);                                             \
        pc_update(c, 3);                                                     \
                                                                             \
        log_printf(#inst_name " " logfmt "\n", (src0str), (src1str),         \
                   (src2str));                                               \
        log_printf("\t%04x = %04x " #op " %04x\n", res, lhs, rhs);           \
        log_printf("\t%s <= %04x\n", (src0str), res);                        \
        log_printf("\tPC <= %04x\n", pc_read(c));                            \
    }
#define DEFINE_INST24_RRR(inst_name, op, calc_expr)            \
    DEFINE_INST24_ARITH(inst_name, op,          /**/           \
                        get_bits(inst, 8, 11),  /* src0/rd */  \
                        get_bits(inst, 12, 15), /* src1/rs1 */ \
                        get_bits(inst, 16, 19), /* src2/rs2 */ \
                        reg_read(c, src1),      /* lhs */      \
                        reg_read(c, src2),      /* rhs */      \
                        calc_expr,              /**/           \
                        "%s, %s, %s",           /* logfmt */   \
                        reg2str(src0),          /* rd */       \
                        reg2str(src1),          /* rs1 */      \
                        reg2str(src2) /* rs2 */)
#define DEFINE_INST24_RRSimm8(inst_name, op, calc_expr)                 \
    DEFINE_INST24_ARITH(inst_name, op,                   /**/           \
                        get_bits(inst, 8, 11),           /* src0/rd */  \
                        get_bits(inst, 12, 15),          /* src1/rs1 */ \
                        sext(8, get_bits(inst, 16, 23)), /* src2/imm */ \
                        reg_read(c, src1),               /* lhs */      \
                        src2,                            /* rhs */      \
                        calc_expr,                       /**/           \
                        "%s, %s, %d",                    /* logfmt */   \
                        reg2str(src0),                   /* rd */       \
                        reg2str(src1),                   /* rs1 */      \
                        (int16_t)src2 /* imm */)
#define DEFINE_INST24_RRUimm8(inst_name, op, calc_expr)        \
    DEFINE_INST24_ARITH(inst_name, op,          /**/           \
                        get_bits(inst, 8, 11),  /* src0/rd */  \
                        get_bits(inst, 12, 15), /* src1/rs1 */ \
                        get_bits(inst, 16, 23), /* src2/imm */ \
                        reg_read(c, src1),      /* lhs */      \
                        src2,                   /* rhs */      \
                        calc_expr,              /**/           \
                        "%s, %s, %d",           /* logfmt */   \
                        reg2str(src0),          /* rd */       \
                        reg2str(src1),          /* rs1 */      \
                        (int16_t)src2 /* imm */)
#define DEFINE_INST24_RRUimm4(inst_name, op, calc_expr) \
    DEFINE_INST24_RRUimm8(inst_name, op, calc_expr)
#define DEFINE_INST24_STORE(inst_name, mem_write_expr)                         \
    static void inst_##inst_name(struct cpu *c, uint32_t inst)                 \
    {                                                                          \
        uint16_t rs = get_bits(inst, 8, 11), rd = get_bits(inst, 12, 15);      \
        uint16_t imm =                                                         \
            sext(10, get_bits(inst, 16, 23) | (get_bits(inst, 6, 7) << 8));    \
                                                                               \
        uint16_t base = reg_read(c, rd), disp = imm, val = reg_read(c, rs);    \
        uint16_t addr = base + disp;                                           \
        (mem_write_expr);                                                      \
        pc_update(c, 3);                                                       \
                                                                               \
        log_printf(#inst_name " %s, %d(%s)\n", reg2str(rd), (int16_t)imm,      \
                   reg2str(rs));                                               \
        log_printf("\t[%04x = %04x + %04x] <= %04x\n", addr, base, disp, val); \
        log_printf("\tPC <= %04x\n", pc_read(c));                              \
    }
#define DEFINE_INST24_LOAD(inst_name, mem_read_expr)                          \
    static void inst_##inst_name(struct cpu *c, uint32_t inst)                \
    {                                                                         \
        uint16_t rd = get_bits(inst, 8, 11), rs = get_bits(inst, 12, 15);     \
        uint16_t imm =                                                        \
            sext(10, get_bits(inst, 16, 23) | (get_bits(inst, 6, 7) << 8));   \
                                                                              \
        uint16_t base = reg_read(c, rs), disp = imm;                          \
        uint16_t addr = base + disp;                                          \
        uint16_t val = (mem_read_expr);                                       \
                                                                              \
        reg_write(c, rd, val);                                                \
        pc_update(c, 3);                                                      \
                                                                              \
        log_printf(#inst_name " %s, %d(%s)\n", reg2str(rd), (int16_t)imm,     \
                   reg2str(rs));                                              \
        log_printf("\t%04x = [%04x = %04x + %04x]\n", val, addr, base, disp); \
        log_printf("\t%s <= %04x\n", reg2str(rd), val);                       \
        log_printf("\tPC <= %04x\n", pc_read(c));                             \
    }
#define DEFINE_INST24_BCC(inst_name, op, calc_expr)                           \
    static void inst_##inst_name(struct cpu *c, uint32_t inst)                \
    {                                                                         \
        uint16_t rs2 = get_bits(inst, 8, 11), rs1 = get_bits(inst, 12, 15);   \
        uint16_t imm =                                                        \
            sext(10, get_bits(inst, 16, 23) | (get_bits(inst, 6, 7) << 8));   \
                                                                              \
        uint16_t lhs = reg_read(c, rs1), rhs = reg_read(c, rs2);              \
        uint16_t res = (calc_expr);                                           \
                                                                              \
        if (res)                                                              \
            pc_write(c, pc_read(c) + imm);                                    \
        else                                                                  \
            pc_update(c, 3);                                                  \
                                                                              \
        log_printf(#inst_name " %s, %s, %d\n", reg2str(rs1), reg2str(rs2),    \
                   (int16_t)imm);                                             \
        log_printf("\t%s = %04x " #op " %04x\n", res ? "true" : "false", lhs, \
                   rhs);                                                      \
        log_printf("\tPC <= %04x\n", pc_read(c));                             \
    }

#include "inst24.inc"

// src0 op= src1
#define DEFINE_INST16_ARITH(inst_name, op, src0_expr, src1_expr, lhs_expr, \
                            rhs_expr, calc_expr, logfmt, src0str, src1str) \
    static void inst_##inst_name##2(struct cpu * c, uint16_t inst)         \
    {                                                                      \
        uint16_t src0 = (src0_expr), src1 = (src1_expr);                   \
                                                                           \
        uint16_t lhs = (lhs_expr), rhs = (rhs_expr);                       \
        uint16_t res = (calc_expr);                                        \
                                                                           \
        reg_write(c, src0, res);                                           \
        pc_update(c, 2);                                                   \
                                                                           \
        log_printf(#inst_name "2 " logfmt "\n", src0str, src1str);         \
        log_printf("\t%04x = %04x " #op " %04x\n", res, lhs, rhs);         \
        log_printf("\t%s <= %04x\n", src0str, res);                        \
        log_printf("\tPC <= %04x\n", pc_read(c));                          \
    }
#define DEFINE_INST16_RR(inst_name, op, calc_expr)            \
    DEFINE_INST16_ARITH(inst_name, op,          /**/          \
                        get_bits(inst, 8, 11),  /* src0/rd */ \
                        get_bits(inst, 12, 15), /* src1/rs */ \
                        reg_read(c, src0),      /* lhs */     \
                        reg_read(c, src1),      /* rhs */     \
                        calc_expr,              /**/          \
                        "%s, %s",               /* logfmt */  \
                        reg2str(src0),          /* rd */      \
                        reg2str(src1) /* rs */)
#define DEFINE_INST16_RSimm6(inst_name, op, calc_expr)           \
    DEFINE_INST16_ARITH(                                         \
        inst_name, op,         /**/                              \
        get_bits(inst, 8, 11), /* src0/rd */                     \
        sext(6, get_bits(inst, 12, 15) |                         \
                    (get_bits(inst, 6, 7) << 4)), /* src1/imm */ \
        reg_read(c, src0),                        /* lhs */      \
        src1,                                     /* rhs */      \
        calc_expr,                                /**/           \
        "%s, %d",                                 /* logfmt */   \
        reg2str(src0),                            /* rd */       \
        (int16_t)src1 /* rs */)
#define DEFINE_INST16_RUimm6(inst_name, op, calc_expr)                       \
    DEFINE_INST16_ARITH(                                                     \
        inst_name, op,                                        /**/           \
        get_bits(inst, 8, 11),                                /* src0/rd */  \
        get_bits(inst, 12, 15) | (get_bits(inst, 6, 7) << 4), /* src1/imm */ \
        reg_read(c, src0),                                    /* lhs */      \
        src1,                                                 /* rhs */      \
        calc_expr,                                            /**/           \
        "%s, %d",                                             /* logfmt */   \
        reg2str(src0),                                        /* rd */       \
        (int16_t)src1 /* rs */)
#define DEFINE_INST16_RUimm4(inst_name, op, calc_expr) \
    DEFINE_INST16_RUimm6(inst_name, op, calc_expr)
#include "inst16.inc"

static void inst_li(struct cpu *c, uint32_t inst)
{
    uint16_t rd = get_bits(inst, 8, 11);
    uint16_t imm =
        sext(10, get_bits(inst, 16, 23) | (get_bits(inst, 6, 7) << 8));

    reg_write(c, rd, imm);
    pc_update(c, 3);

    log_printf("li %s, %d\n", reg2str(rd), (int16_t)imm);
    log_printf("\t%s <= %04x\n", reg2str(rd), imm);
    log_printf("\tPC <= %04x\n", pc_read(c));
}

const struct inst24_info inst_list_24[] = {
    {"xxxx_xxxx_xxxx_xxxx_xx01_0101", inst_lw},   // LW
    {"xxxx_xxxx_xxxx_xxxx_xx10_0101", inst_lb},   // LB
    {"xxxx_xxxx_xxxx_xxxx_xx00_0101", inst_lbu},  // LBU
    {"xxxx_xxxx_xxxx_xxxx_xxx1_1101", inst_sw},   // SW
    {"xxxx_xxxx_xxxx_xxxx_xx00_1101", inst_sb},   // SB
    {"xxxx_xxxx_xxxx_xxxx_xx11_0101", inst_li},   // LI

    {"xxxx_xxxx_xxxx_xxxx_xx00_0001", inst_add},  // ADD
    {"xxxx_xxxx_xxxx_xxxx_xx00_1001", inst_sub},  // SUB
    {"xxxx_xxxx_xxxx_xxxx_xx01_0001", inst_and},  // AND
    {"xxxx_xxxx_xxxx_xxxx_xx01_1001", inst_xor},  // XOR
    {"xxxx_xxxx_xxxx_xxxx_xx10_0001", inst_or},   // OR
    {"xxxx_xxxx_xxxx_xxxx_xx10_1001", inst_lsl},  // LSL
    {"xxxx_xxxx_xxxx_xxxx_xx11_0001", inst_lsr},  // LSR
    {"xxxx_xxxx_xxxx_xxxx_xx11_1001", inst_asr},  // ASR

    {"xxxx_xxxx_xxxx_xxxx_1100_0011", inst_addi},  // ADDI
    {"xxxx_xxxx_xxxx_xxxx_0101_0011", inst_andi},  // ANDI
    {"xxxx_xxxx_xxxx_xxxx_0101_1011", inst_xori},  // XORI
    {"xxxx_xxxx_xxxx_xxxx_0110_0011", inst_ori},   // ORI
    {"0000_xxxx_xxxx_xxxx_0010_1011", inst_lsli},  // LSLI
    {"0000_xxxx_xxxx_xxxx_0011_0011", inst_lsri},  // LSRI
    {"0000_xxxx_xxxx_xxxx_0011_1011", inst_asri},  // ASRI

    {"xxxx_xxxx_xxxx_xxxx_xx00_1111", inst_beq},   // BEQ
    {"xxxx_xxxx_xxxx_xxxx_xx10_1111", inst_bne},   // BNE
    {"xxxx_xxxx_xxxx_xxxx_xx11_0111", inst_blt},   // BLT
    {"xxxx_xxxx_xxxx_xxxx_xx01_0111", inst_bltu},  // BLTU
    {"xxxx_xxxx_xxxx_xxxx_xx11_1111", inst_ble},   // BLE
    {"xxxx_xxxx_xxxx_xxxx_xx01_1111", inst_bleu},  // BLEU

    {NULL, NULL}  // Terminator
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

    {"xxxx_xxxx_0010_1010", inst_lsli2},  // LSLI2
    {"xxxx_xxxx_0011_0010", inst_lsri2},  // LSRI2
    {"xxxx_xxxx_xx00_0010", inst_addi2},  // ADDI2
    {"xxxx_xxxx_xx01_0010", inst_andi2},  // ANDI2
    {NULL, NULL}                          // Terminator
};
