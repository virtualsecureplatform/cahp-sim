#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "bitpat.h"
#include "cpu.h"
#include "inst.h"
#include "log.h"

void pc_update(struct cpu *c, uint16_t offset)
{
    c->pc += offset;
    log_printf("PC <= 0x%04X ", c->pc);
}

void pc_write(struct cpu *c, uint16_t addr)
{
    c->pc = addr;
    log_printf("PC <= 0x%04X ", c->pc);
}

uint16_t pc_read(struct cpu *c) { return c->pc; }

void reg_write(struct cpu *c, uint8_t reg_idx, uint16_t data)
{
    c->reg[reg_idx] = data;
    log_printf("Reg x%d <= 0x%04X ", reg_idx, data);
}

uint16_t reg_read(struct cpu *c, uint8_t reg_idx) { return c->reg[reg_idx]; }

void mem_write_b(struct cpu *c, uint16_t addr, uint8_t data)
{
    log_printf("DataRam[0x%04X] <= 0x%04X ", addr, data);

    assert(addr < DATA_RAM_SIZE && "RAM write to invalid address!");

    c->data_ram[addr] = data;
}

void mem_write_w(struct cpu *c, uint16_t addr, uint16_t data)
{
    log_printf("DataRam[0x%04X] <= 0x%04X ", addr, data & 0xFF);
    log_printf("DataRam[0x%04X] <= 0x%04X ", addr + 1, data >> 8);

    assert(addr < DATA_RAM_SIZE - 1 && "RAM write to invalid address!");
    c->data_ram[addr] = data & 0xFF;
    c->data_ram[addr + 1] = data >> 8;
}

uint8_t mem_read_b(struct cpu *c, uint16_t addr)
{
    assert(addr < DATA_RAM_SIZE && "RAM read from invalid address!");
    return c->data_ram[addr];
}

uint16_t mem_read_w(struct cpu *c, uint16_t addr)
{
    assert(addr < DATA_RAM_SIZE - 1 && "RAM read from invalid address!");
    return c->data_ram[addr] + (c->data_ram[addr + 1] << 8);
}

uint16_t rom_read_w(struct cpu *c)
{
    assert(c->pc < INST_ROM_SIZE - 1 && "ROM read from invalid address!");
    return c->inst_rom[c->pc] + (c->inst_rom[c->pc + 1] << 8);
}

uint16_t get_bits(uint16_t t, int s, int e)
{
    int bit_len = e - s;
    uint32_t bit_mask = 1;
    for (int i = 0; i < bit_len; i++) {
        bit_mask = (bit_mask << 1) + 1;
    }
    return (t >> s) & bit_mask;
}

uint16_t sign_ext(uint16_t t, uint8_t sign_bit)
{
    uint16_t sign_v = 0;
    uint16_t sign = get_bits(t, sign_bit, sign_bit);
    for (int i = sign_bit; i < 16; i++) {
        sign_v |= (sign << i);
    }
    return t | sign_v;
}

uint8_t flag_zero(uint16_t res) { return res == 0; }

uint8_t flag_sign(uint16_t res) { return get_bits(res, 15, 15); }

uint8_t flag_overflow(uint16_t s1, uint16_t s2, uint16_t res)
{
    uint8_t s1_sign = get_bits(s1, 15, 15);
    uint8_t s2_sign = get_bits(s2, 15, 15);
    uint8_t res_sign = get_bits(res, 15, 15);
    return ((s1_sign ^ s2_sign) == 0) & ((s2_sign ^ res_sign) == 1);
}

void inst_lw(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:LW\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint16_t imm = rom_read_w(c);
    uint16_t s_data = reg_read(c, rs);
    uint16_t res = imm + s_data;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(imm, s_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    reg_write(c, rd, mem_read_w(c, res & 0xFFFF));
    pc_update(c, 2);
}

void inst_lwsp(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:LWSP\t");

    // uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t imm = (get_bits(inst, 4, 11) << 1);
    uint16_t d_data = reg_read(c, 1);
    uint16_t res = imm + d_data;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(imm, d_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    reg_write(c, rd, mem_read_w(c, res & 0xFFFF));
    pc_update(c, 2);
}

void inst_lbu(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:LBU\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint16_t imm = rom_read_w(c);
    uint16_t s_data = reg_read(c, rs);
    uint16_t res = imm + s_data;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(imm, s_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    reg_write(c, rd, mem_read_b(c, res & 0xFFFF));
    pc_update(c, 2);
}

void inst_lb(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:LB\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint16_t imm = rom_read_w(c);
    uint16_t s_data = reg_read(c, rs);
    uint16_t res = imm + s_data;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(imm, s_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    reg_write(c, rd, sign_ext(mem_read_b(c, res & 0xFFFF), 7));
    pc_update(c, 2);
}

void inst_sw(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:SW\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint16_t imm = rom_read_w(c);
    uint16_t d_data = reg_read(c, rd);
    uint16_t res = imm + d_data;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(imm, d_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    mem_write_w(c, res & 0xFFFF, reg_read(c, rs));
    pc_update(c, 2);
}

void inst_swsp(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:SWSP\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t imm = (get_bits(inst, 8, 11) << 5) + (rd << 1);
    uint16_t s_data = reg_read(c, 1);
    uint16_t res = s_data + imm;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(imm, s_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    mem_write_w(c, res & 0xFFFF, reg_read(c, rs));
    pc_update(c, 2);
}

void inst_sb(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:SB\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint16_t imm = rom_read_w(c);
    uint16_t d_data = reg_read(c, rd);
    uint16_t res = imm + d_data;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(imm, d_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    mem_write_b(c, res & 0xFFFF, reg_read(c, rs) & 0xFF);
    pc_update(c, 2);
}

void inst_mov(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:MOV\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    reg_write(c, rd, s_data);
    c->flag_carry = 0;
    c->flag_sign = flag_sign(s_data & 0xFFFF);
    c->flag_overflow = 0;
    c->flag_zero = flag_zero(s_data & 0xFFFF);
    pc_update(c, 2);
}

void inst_add(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:ADD\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    uint16_t d_data = reg_read(c, rd);
    uint32_t res = s_data + d_data;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(s_data, d_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    reg_write(c, rd, res & 0xFFFF);
    pc_update(c, 2);
}

void inst_sub(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:SUB\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = (~reg_read(c, rs)) + 1;
    uint16_t d_data = reg_read(c, rd);
    uint32_t res = s_data + d_data;
    if (res > 0xFFFF || s_data == 0) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(s_data, d_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    reg_write(c, rd, res & 0xFFFF);
    pc_update(c, 2);
}

void inst_and(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:AND\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    uint16_t d_data = reg_read(c, rd);
    uint16_t res_w = s_data & d_data;
    reg_write(c, rd, res_w);
    c->flag_carry = 0;
    c->flag_sign = flag_sign(res_w);
    c->flag_overflow = flag_overflow(s_data, d_data, res_w);
    c->flag_zero = flag_zero(res_w);
    pc_update(c, 2);
}

void inst_or(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:OR\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    uint16_t d_data = reg_read(c, rd);
    uint16_t res_w = s_data | d_data;
    reg_write(c, rd, res_w);
    c->flag_carry = 0;
    c->flag_sign = flag_sign(res_w);
    c->flag_overflow = flag_overflow(s_data, d_data, res_w);
    c->flag_zero = flag_zero(res_w);
    pc_update(c, 2);
}

void inst_xor(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:XOR\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    uint16_t d_data = reg_read(c, rd);
    uint16_t res_w = s_data ^ d_data;
    reg_write(c, rd, res_w);
    c->flag_carry = 0;
    c->flag_sign = flag_sign(res_w);
    c->flag_overflow = flag_overflow(s_data, d_data, res_w);
    c->flag_zero = flag_zero(res_w);
    pc_update(c, 2);
}

void inst_lsl(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:LSL\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    uint16_t d_data = reg_read(c, rd);
    uint16_t res_w = d_data << s_data;
    reg_write(c, rd, res_w);
    c->flag_carry = 0;
    c->flag_sign = flag_sign(res_w);
    c->flag_overflow = flag_overflow(s_data, d_data, res_w);
    c->flag_zero = flag_zero(res_w);
    pc_update(c, 2);
}

void inst_lsr(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:LSR\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    uint16_t d_data = reg_read(c, rd);
    uint16_t res_w = d_data >> s_data;
    reg_write(c, rd, res_w);
    c->flag_carry = 0;
    c->flag_sign = flag_sign(res_w);
    c->flag_overflow = flag_overflow(s_data, d_data, res_w);
    c->flag_zero = flag_zero(res_w);
    pc_update(c, 2);
}

void inst_asr(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:ASR\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = reg_read(c, rs);
    uint16_t d_data = reg_read(c, rd);
    uint16_t res_w = ((int16_t)d_data) >> s_data;
    reg_write(c, rd, res_w);
    c->flag_carry = 0;
    c->flag_sign = flag_sign(res_w);
    c->flag_overflow = flag_overflow(s_data, d_data, res_w);
    c->flag_zero = flag_zero(res_w);
    pc_update(c, 2);
}

void inst_cmp(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:CMP\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = (~reg_read(c, rs)) + 1;
    uint16_t d_data = reg_read(c, rd);
    uint32_t res = s_data + d_data;
    if (res > 0xFFFF || s_data == 0) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(s_data, d_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    pc_update(c, 2);
}

void inst_li(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:LI\t");

    // uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);
    pc_update(c, 2);

    uint16_t imm = rom_read_w(c);
    c->flag_carry = 0;
    c->flag_sign = flag_sign(imm & 0xFFFF);
    c->flag_overflow = 0;
    c->flag_zero = flag_zero(imm & 0xFFFF);
    reg_write(c, rd, imm);
    pc_update(c, 2);
}

void inst_addi(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:ADDI\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = sign_ext(rs, 3);
    uint16_t d_data = reg_read(c, rd);
    uint32_t res = s_data + d_data;
    if (res > 0xFFFF) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(s_data, d_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    reg_write(c, rd, res & 0xFFFF);
    pc_update(c, 2);
}

void inst_cmpi(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:CMPI\t");

    uint8_t rs = get_bits(inst, 4, 7);
    uint8_t rd = get_bits(inst, 0, 3);

    uint16_t s_data = (~sign_ext(rs, 3)) + 1;
    uint16_t d_data = reg_read(c, rd);
    uint32_t res = s_data + d_data;
    if (res > 0xFFFF || s_data == 0) {
        c->flag_carry = 0;
    }
    else {
        c->flag_carry = 1;
    }
    c->flag_sign = flag_sign(res & 0xFFFF);
    c->flag_overflow = flag_overflow(s_data, d_data, res & 0xFFFF);
    c->flag_zero = flag_zero(res & 0xFFFF);
    pc_update(c, 2);
}

void inst_j(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:J\t");

    pc_update(c, 2);

    uint16_t imm = rom_read_w(c);
    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
    pc_update(c, imm);
}

void inst_jal(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JAL\t");

    pc_update(c, 2);

    uint16_t imm = rom_read_w(c);
    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
    reg_write(c, 0, pc_read(c) + 2);
    pc_update(c, imm);
}

void inst_jalr(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JALR\t");

    uint8_t rs = get_bits(inst, 4, 7);

    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
    reg_write(c, 0, pc_read(c) + 2);
    pc_write(c, reg_read(c, rs));
}

void inst_jr(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JR\t");

    uint8_t rs = get_bits(inst, 4, 7);

    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
    pc_write(c, reg_read(c, rs));
}

void inst_jl(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JL\t");

    uint16_t imm = sign_ext(get_bits(inst, 0, 6) << 1, 7);

    if (c->flag_sign != c->flag_overflow) {
        pc_update(c, imm);
    }
    else {
        pc_update(c, 2);
    }
    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
}

void inst_jle(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JLE\t");

    uint16_t imm = sign_ext(get_bits(inst, 0, 6) << 1, 7);

    if (c->flag_sign != c->flag_overflow || c->flag_zero == 1) {
        pc_update(c, imm);
    }
    else {
        pc_update(c, 2);
    }
    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
}

void inst_je(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JE\t");

    uint16_t imm = sign_ext(get_bits(inst, 0, 6) << 1, 7);

    if (c->flag_zero == 1) {
        pc_update(c, imm);
    }
    else {
        pc_update(c, 2);
    }
    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
}

void inst_jne(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JNE\t");

    uint16_t imm = sign_ext(get_bits(inst, 0, 6) << 1, 7);

    if (c->flag_zero == 0) {
        pc_update(c, imm);
    }
    else {
        pc_update(c, 2);
    }
    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
}

void inst_jb(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JB\t");

    uint16_t imm = (sign_ext(get_bits(inst, 0, 6) << 1, 7));

    if (c->flag_carry == 1) {
        pc_update(c, imm);
    }
    else {
        pc_update(c, 2);
    }
    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
}

void inst_jbe(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:JBE\t");

    uint16_t imm = sign_ext(get_bits(inst, 0, 6) << 1, 7);

    if (c->flag_carry == 1 || c->flag_zero == 1) {
        pc_update(c, imm);
    }
    else {
        pc_update(c, 2);
    }
    c->flag_carry = 0;
    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
}

void inst_nop(struct cpu *c, uint16_t inst)
{
    log_printf("Inst:NOP\t");

    c->flag_sign = 0;
    c->flag_overflow = 0;
    c->flag_zero = 0;
    c->flag_carry = 0;
    pc_update(c, 2);
}

const struct inst_data inst_list[] = {
    {"0b1011_0010_xxxx_xxxx", inst_lw},    // LW
    {"0b1010_xxxx_xxxx_xxxx", inst_lwsp},  // LWSP
    {"0b1011_1010_xxxx_xxxx", inst_lbu},   // LBU
    {"0b1011_1110_xxxx_xxxx", inst_lb},    // LB
    {"0b1001_0010_xxxx_xxxx", inst_sw},    // SW
    {"0b1000_xxxx_xxxx_xxxx", inst_swsp},  // SWSP
    {"0b1001_1010_xxxx_xxxx", inst_sb},    // SB
    {"0b1110_0000_xxxx_xxxx", inst_mov},   // MOV
    {"0b1110_0010_xxxx_xxxx", inst_add},   // ADD
    {"0b1110_0011_xxxx_xxxx", inst_sub},   // SUB
    {"0b1110_0100_xxxx_xxxx", inst_and},   // AND
    {"0b1110_0101_xxxx_xxxx", inst_or},    // OR
    {"0b1110_0110_xxxx_xxxx", inst_xor},   // XOR
    {"0b1110_1001_xxxx_xxxx", inst_lsl},   // LSL
    {"0b1110_1010_xxxx_xxxx", inst_lsr},   // LSR
    {"0b1110_1101_xxxx_xxxx", inst_asr},   // ASR
    {"0b1100_0011_xxxx_xxxx", inst_cmp},   // CMP
    {"0b0111_1000_xxxx_xxxx", inst_li},    // LI
    {"0b1111_0010_xxxx_xxxx", inst_addi},  // ADDI
    {"0b1101_0011_xxxx_xxxx", inst_cmpi},  // CMPI
    {"0b0101_0010_0000_0000", inst_j},     // J
    {"0b0111_0011_0000_0000", inst_jal},   // JAL
    {"0b0110_0001_xxxx_0000", inst_jalr},  // JALR
    {"0b0100_0000_xxxx_0000", inst_jr},    // JR
    {"0b0100_0100_0xxx_xxxx", inst_jl},    // JL
    {"0b0100_0100_1xxx_xxxx", inst_jle},   // JLE
    {"0b0100_0101_0xxx_xxxx", inst_je},    // JE
    {"0b0100_0101_1xxx_xxxx", inst_jne},   // JNE
    {"0b0100_0110_0xxx_xxxx", inst_jb},    // JB
    {"0b0100_0110_1xxx_xxxx", inst_jbe},   // JBE
    {"0b0000_0000_0000_0000", inst_nop},   // NOP
    {NULL, NULL}                           // Terminator
};
