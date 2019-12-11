add_executable(cahp-sim)
install(TARGETS cahp-sim DESTINATION bin)
set_target_properties(cahp-sim PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
)
target_compile_options(cahp-sim
    PRIVATE -Wall -Wextra
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
