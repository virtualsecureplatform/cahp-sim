///// M-Instruction
DEFINE_INST24_LOAD(lw,  mem_read_w(c, addr))
DEFINE_INST24_LOAD(lb,  sext(8, mem_read_b(c, addr)))
DEFINE_INST24_LOAD(lbu, mem_read_b(c, addr))
DEFINE_INST24_STORE(sw, mem_write_w(c, addr, val))
DEFINE_INST24_STORE(sb, mem_write_b(c, addr, val))

///// R-Instruction
DEFINE_INST24_RRR(add, +,   lhs + rhs)
DEFINE_INST24_RRR(sub, -,   lhs - rhs)
DEFINE_INST24_RRR(and, &,   lhs & rhs)
DEFINE_INST24_RRR(xor, ^,   lhs ^ rhs)
DEFINE_INST24_RRR(or,  |,   lhs | rhs)
DEFINE_INST24_RRR(lsl, <<,  lhs << rhs)
DEFINE_INST24_RRR(lsr, >>,  lhs >> rhs)
DEFINE_INST24_RRR(asr, >>>, asr(lhs, rhs))

///// I-Instruction
DEFINE_INST24_RRSimm10(addi, +,   lhs + rhs)
DEFINE_INST24_RRSimm10(andi, &,   lhs & rhs)
DEFINE_INST24_RRSimm10(xori, ^,   lhs ^ rhs)
DEFINE_INST24_RRSimm10(ori,  |,   lhs | rhs)
DEFINE_INST24_RRUimm4 (lsli, <<,  lhs << rhs)
DEFINE_INST24_RRUimm4 (lsri, >>,  lhs >> rhs)
DEFINE_INST24_RRUimm4 (asri, >>>, asr(lhs, rhs))

///// J-Instruction
DEFINE_INST24_BCC(beq,  ==,           lhs ==          rhs)
DEFINE_INST24_BCC(bne,  !=,           lhs !=          rhs)
DEFINE_INST24_BCC(blt,  <s,  (int16_t)lhs <  (int16_t)rhs)
DEFINE_INST24_BCC(bltu, <u,           lhs <           rhs)
DEFINE_INST24_BCC(ble,  <=s, (int16_t)lhs <= (int16_t)rhs)
DEFINE_INST24_BCC(bleu, <=u,          lhs <=          rhs)
