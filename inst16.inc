///// R-Instruction
DEFINE_INST16_RR(add, +,   lhs + rhs)
DEFINE_INST16_RR(sub, -,   lhs - rhs)
DEFINE_INST16_RR(and, &,   lhs & rhs)
DEFINE_INST16_RR(xor, ^,   lhs ^ rhs)
DEFINE_INST16_RR(or,  |,   lhs | rhs)
DEFINE_INST16_RR(lsl, <<,  lhs << rhs)
DEFINE_INST16_RR(lsr, >>,  lhs >> rhs)
// NOTE: According to N1548, the value of (((int32_t)lhs) >> rhs) is
// implementation-defined if lhs is negative.
DEFINE_INST16_RR(asr, >>>, (lhs >> 15) == 0
                            ? /* positive */ lhs >> rhs
                            : /* negative */ (~0 << (16 - rhs)) | (lhs >> rhs))
