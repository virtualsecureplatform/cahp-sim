#ifndef INST_H
#define INST_H

#include <stdint.h>

#include "cpu.h"

typedef void (*inst24_handler)(struct cpu *c, uint32_t inst);
typedef void (*inst16_handler)(struct cpu *c, uint16_t inst);

struct inst24_info {
    char *bit_pattern;
    inst24_handler func;
};

struct inst16_info {
    char *bit_pattern;
    inst16_handler func;
};

extern const struct inst24_info inst_list_24[];
extern const struct inst16_info inst_list_16[];

#endif
