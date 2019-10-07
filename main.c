#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitpat.h"
#include "cpu.h"
#include "elf_parser.h"
#include "inst.h"
#include "log.h"

#include <getopt.h>
#include <unistd.h>

extern int flag_quiet;

void print_usage(FILE *fh)
{
    fprintf(fh,
            "Usage: cahp-sim [-q] [-m] [-t ROM] [-d RAM] [FILENAME] NCYCLES\n");
    fprintf(fh, "Options:\n");
    fprintf(fh, "  -q     : No log print.\n");
    fprintf(fh, "  -m     : Dump memory.\n");
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

int main(int argc, char *argv[])
{
    struct cpu cpu;

    int flag_load_elf = 1, flag_memory_dump = 0, opt;
    while ((opt = getopt(argc, argv, "qmt:")) != -1) {
        switch (opt) {
        case 'q': flag_quiet = 1; break;

        case 'm': flag_memory_dump = 1; break;

        case 't':
            flag_load_elf = 0;
            cpu_init_from_initconf(&cpu, optarg);
            break;

        default: print_usage_to_exit();
        }
    }

    if (optind >= argc) print_usage_to_exit();

    int iarg = optind;
    if (flag_load_elf) {
        cpu_init(&cpu);
        elf_parse(&cpu, argv[iarg++]);
    }

    int ncycles = 0;
    if (iarg >= argc) print_usage_to_exit();
    ncycles = atoi(argv[iarg]);

    for (int i = 0; i < ncycles; i++) {
        cpu_tick(&cpu);

        // if (flag_memory_dump) {
        //    dump_memory(stdout, cpu.data_ram, DATA_RAM_SIZE);
        //    printf("\n");
        //}
    }

    for (int i = 0; i < 16; i++) {
        uint16_t val = reg_read(&cpu, i);
        printf("x%d=%d\t", i, val);
    }
    printf("pc=%d\n", pc_read(&cpu));

    return 0;
}
