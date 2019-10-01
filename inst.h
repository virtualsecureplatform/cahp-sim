#ifndef INST_H
#define INST_H

#include "cpu.h"

#include <stdint.h>

typedef void (*inst_func)(struct cpu *c, uint16_t inst);

struct inst_data {
    char *bit_pattern;
    inst_func func;
};

extern const struct inst_data inst_list_24[];
extern const struct inst_data inst_list_16[];

#endif
