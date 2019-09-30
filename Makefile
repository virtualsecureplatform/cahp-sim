main: main.c elf_parser.c bitpat.c log.c inst.c
	clang -Wall -O0 -g3 -std=c11 -Wpedantic -o $@ $^
clean:
	rm main
test:
	./test.sh

.PHONY: clean test
