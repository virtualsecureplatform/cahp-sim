#ifndef INST_H
#define INST_H

#include <stdint.h>
#include "inst.h"

typedef void (*inst_func)(struct cpu *c, uint16_t inst);

struct inst_data {
    char *bit_pattern;
    inst_func func;
};

extern const struct inst_data inst_list[];

uint16_t pc_read(struct cpu *c);
uint16_t reg_read(struct cpu *c, uint8_t reg_idx);
uint16_t rom_read_w(struct cpu *c);

#endif
