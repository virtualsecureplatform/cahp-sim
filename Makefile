cahp-sim: main.c elf_parser.c bitpat.c log.c inst.c cpu.c inst16.inc inst24.inc
	clang -Wall -O0 -g3 -std=c11 -Wpedantic -o $@ $(filter %.c, $^)
clean:
	rm cahp-sim
test: cahp-sim
	./test.sh

.PHONY: clean test
