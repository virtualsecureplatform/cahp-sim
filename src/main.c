#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "bitpat.h"
#include "cpu.h"
#include "elf_parser.h"
#include "inst.h"
#include "log.h"

extern int flag_quiet;

void print_usage(FILE *fh)
{
    fprintf(fh,
            "Usage: cahp-sim [-q] [-m] [-c NCYCLES] [-t INITCONF] "
            "[FILENAME OPTIONS-TO-PROG...]\n");
    fprintf(fh, "Options:\n");
    fprintf(fh, "  -q          : No log print.\n");
    fprintf(fh, "  -m          : Dump memory.\n");
    fprintf(fh, "  -c NCYCLES  : Number of cycles.\n");
    fprintf(fh, "  -t INITCONF : Specify initconf.\n");
}

_Noreturn void print_usage_to_exit(void)
{
    print_usage(stderr);
    exit(1);
}

void set_bytes_from_str(uint8_t *dst, const char *const src, int N)
{
    char *buf = (char *)malloc(strlen(src) + 1);
    strcpy(buf, src);

    int idst = 0;
    char *tp;
    tp = strtok(buf, " ");
    while (tp != NULL) {
        assert(idst < N && "Too large data!");
        uint8_t val = strtol(tp, NULL, 16);
        dst[idst++] = val;
        tp = strtok(NULL, " ");
    }

    free(buf);
}

void dump_memory(FILE *fh, uint8_t *mem, int size)
{
    for (int i = 0; i < size; i++) {
        fprintf(fh, "%02x ", mem[i]);
        if (i % 16 == 15) fprintf(fh, "\n");
    }
}

void place_cmd_args(struct cpu *c, int argc, char **argv)
{
    uint16_t *sargv = malloc(sizeof(uint16_t) * (argc + 1));
    // N1548 5.1.2.2.1 2
    // argv[argc] shall be a null pointer.
    sargv[argc] = 0;

    // Copy **argv to RAM.
    uint16_t index = DATA_RAM_SIZE - 2;
    for (int i = argc - 1; i >= 0; i--) {
        int len = strlen(argv[i]);
        for (int j = len /* including '\0' */; j >= 0; j--) {
            index--;
            mem_write_b(c, index, (uint8_t)argv[i][j]);
        }
        sargv[i] = index;
    }
    if (index % 2 == 1) index--;

    // Copy *argv to RAM.
    for (int i = argc /* including ending 0*/; i >= 0; i--) {
        index -= 2;
        mem_write_w(c, index, sargv[i]);
    }

    // Save argc in RAM.
    index -= 2;
    mem_write_w(c, index, argc);

    // Save initial stack pointer in RAM.
    mem_write_w(c, DATA_RAM_SIZE - 2, index);

    free(sargv);
}

int main(int argc, char *argv[])
{
    struct cpu cpu;

    int flag_load_elf = 1, flag_memory_dump = 0, opt;
    uint64_t ncycles = UINT64_MAX;
    while ((opt = getopt(argc, argv, "qmt:c:")) != -1) {
        switch (opt) {
        case 'q': flag_quiet = 1; break;

        case 'm': flag_memory_dump = 1; break;

        case 't':
            flag_load_elf = 0;
            cpu_init_from_initconf(&cpu, optarg);
            break;

        case 'c': ncycles = strtoull(optarg, NULL, 10); break;

        default: print_usage_to_exit();
        }
    }

    // Check the number of arguments
    if (flag_load_elf && optind >= argc) print_usage_to_exit();
    if (!flag_load_elf && optind != argc) print_usage_to_exit();

    if (flag_load_elf) {
        cpu_init(&cpu);
        elf_parse(&cpu, argv[optind]);
        place_cmd_args(&cpu, argc - optind, argv + optind);
    }

    for (uint64_t i = 0; i < ncycles; i++) {
        uint16_t inst16 = rom_read_16(&cpu);

        log_printf("%5" PRIx64 ": ", i);
        cpu_tick(&cpu);

        if (flag_memory_dump) {
            dump_memory(stderr, cpu.data_ram, DATA_RAM_SIZE);
            fprintf(stderr, "\n");
        }

        if (inst16 == 0x000e /* JS 0*/) break;
    }

    for (int i = 0; i < 16; i++) {
        uint16_t val = reg_read(&cpu, i);
        printf("x%d=%d\t", i, val);
    }
    printf("pc=%d\n", pc_read(&cpu));

    return 0;
}
