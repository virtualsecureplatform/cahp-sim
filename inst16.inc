///// R-Instruction
DEFINE_INST16_RR(add, +,   lhs + rhs)
DEFINE_INST16_RR(sub, -,   lhs - rhs)
DEFINE_INST16_RR(and, &,   lhs & rhs)
DEFINE_INST16_RR(xor, ^,   lhs ^ rhs)
DEFINE_INST16_RR(or,  |,   lhs | rhs)
DEFINE_INST16_RR(lsl, <<,  lhs << rhs)
DEFINE_INST16_RR(lsr, >>,  lhs >> rhs)
DEFINE_INST16_RR(asr, >>>, asr(lhs, rhs))

///// I-Instruction
DEFINE_INST16_RUimm4(lsli, <<,  lhs << rhs)
DEFINE_INST16_RUimm4(lsri, >>,  lhs >> rhs)
DEFINE_INST16_RUimm4(asri, >>>, asr(lhs, rhs))
DEFINE_INST16_RSimm6(addi, +,   lhs + rhs)
DEFINE_INST16_RSimm6(andi, &,   lhs & rhs)
