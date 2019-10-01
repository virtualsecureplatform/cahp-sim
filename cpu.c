#include "cpu.h"
#include "bitpat.h"
#include "inst.h"
#include "log.h"

#include <assert.h>
#include <stdlib.h>

void pc_update(struct cpu *c, uint16_t offset) { c->pc += offset; }

void pc_write(struct cpu *c, uint16_t addr) { c->pc = addr; }

uint16_t pc_read(struct cpu *c) { return c->pc; }

void reg_write(struct cpu *c, uint8_t reg_idx, uint16_t data)
{
    c->reg[reg_idx] = data;
}

uint16_t reg_read(struct cpu *c, uint8_t reg_idx) { return c->reg[reg_idx]; }

void mem_write_b(struct cpu *c, uint16_t addr, uint8_t data)
{
    assert(addr < DATA_RAM_SIZE && "RAM write to invalid address!");

    c->data_ram[addr] = data;
}

void mem_write_w(struct cpu *c, uint16_t addr, uint16_t data)
{
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

uint16_t rom_read_16(struct cpu *c)
{
    assert(c->pc < INST_ROM_SIZE - 1 && "ROM read from invalid address!");
    return c->inst_rom[c->pc] + (c->inst_rom[c->pc + 1] << 8);
}

uint32_t rom_read_24(struct cpu *c)
{
    assert(c->pc < INST_ROM_SIZE - 1 && "ROM read from invalid address!");
    uint32_t val = 0;
    val |= c->inst_rom[c->pc];
    val |= (c->inst_rom[c->pc + 1] << 8);
    val |= (c->inst_rom[c->pc + 2] << 16);
    return val;
}

void cpu_tick(struct cpu *c)
{
    uint32_t inst24 = rom_read_24(c);
    for (int i = 0; inst_list_24[i].bit_pattern != NULL; i++) {
        if (!bitpat_match_s(24, inst24, inst_list_24[i].bit_pattern)) continue;
        inst_list_24[i].func(c, inst24);
        return;
    }

    uint16_t inst16 = rom_read_24(c);
    for (int i = 0; inst_list_16[i].bit_pattern != NULL; i++) {
        if (!bitpat_match_s(16, inst16, inst_list_16[i].bit_pattern)) continue;
        inst_list_16[i].func(c, inst16);
        return;
    }

    assert(0 && "Invalid input binary!");
}

void cpu_init(struct cpu *c)
{
    for (int i = 0; i < 16; i++) {
        c->reg[i] = 0;
    }
    for (int i = 0; i < INST_ROM_SIZE; i++) {
        c->inst_rom[i] = 0;
    }
    for (int i = 0; i < DATA_RAM_SIZE; i++) {
        c->data_ram[i] = 0;
    }
    c->pc = 0;
}
