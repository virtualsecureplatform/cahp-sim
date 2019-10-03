///// R-Instruction
DEFINE_INST24_RRR(add, +,   lhs + rhs)
DEFINE_INST24_RRR(sub, -,   lhs - rhs)
DEFINE_INST24_RRR(and, &,   lhs & rhs)
DEFINE_INST24_RRR(xor, ^,   lhs ^ rhs)
DEFINE_INST24_RRR(or,  |,   lhs | rhs)
DEFINE_INST24_RRR(lsl, <<,  lhs << rhs)
DEFINE_INST24_RRR(lsr, >>,  lhs >> rhs)
// NOTE: According to N1548, the value of (((int32_t)lhs) >> rhs) is
// implementation-defined if lhs is negative.
DEFINE_INST24_RRR(asr, >>>, (lhs >> 15) == 0
                            ? /* positive */ lhs >> rhs
                            : /* negative */ (~0 << (16 - rhs)) | (lhs >> rhs))