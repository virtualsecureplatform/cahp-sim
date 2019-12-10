#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#define INST_ROM_SIZE 512
#define DATA_RAM_SIZE 512
struct cpu {
    uint16_t reg[16];
    uint16_t pc;
    uint8_t inst_rom[INST_ROM_SIZE];
    uint8_t data_ram[DATA_RAM_SIZE];
};

void pc_update(struct cpu *c, uint16_t offset);
void pc_write(struct cpu *c, uint16_t addr);
uint16_t pc_read(struct cpu *c);

void reg_write(struct cpu *c, uint8_t reg_idx, uint16_t data);
uint16_t reg_read(struct cpu *c, uint8_t reg_idx);

void mem_write_b(struct cpu *c, uint16_t addr, uint8_t data);
void mem_write_w(struct cpu *c, uint16_t addr, uint16_t data);
uint8_t mem_read_b(struct cpu *c, uint16_t addr);
uint16_t mem_read_w(struct cpu *c, uint16_t addr);

uint16_t rom_read_16(struct cpu *c);
uint32_t rom_read_24(struct cpu *c);

void cpu_init(struct cpu *c);
void cpu_init_from_initconf(struct cpu *c, const char *text);
void cpu_tick(struct cpu *c);

#endif
