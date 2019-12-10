add_executable(cahp-sim)
install(TARGETS cahp-sim DESTINATION bin)
target_compile_options(cahp-sim
    PRIVATE -std=c11 -Wall -Wextra
    PRIVATE $<$<CONFIG:DEBUG>:-O0 -g3>
    PRIVATE $<$<CONFIG:RELEASE>:-O2>)
target_sources(cahp-sim
    PRIVATE
        bitpat.c
        bitpat.h
        cpu.c
        cpu.h
        elf.h
        elf_parser.c
        elf_parser.h
        inst.c
        inst.h
        inst16.inc
        inst24.inc
        log.c
        log.h
        main.c
    )
