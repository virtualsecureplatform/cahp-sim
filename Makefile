cahp-sim: main.c elf_parser.c bitpat.c log.c inst.c cpu.c
	clang -Wall -O0 -g3 -std=c11 -Wpedantic -o $@ $^
clean:
	rm cahp-sim
test: cahp-sim
	./test.sh

.PHONY: clean test
