#!/usr/bin/bash

failwith() {
    echo -e "\e[31m[ERROR]\e[m testentry \"$1\" \"$2\" \"$3\" \"$4\""

    if [ -n "$5" ]; then
        echo "===== RESULT FROM HERE ====="
        echo "$5"
        echo "===== RESULT TO HERE ====="
    fi

    exit 1
}

testentry() {
    res=$(./cahp-sim -t "$2" "$1")
    [ "$?" -eq 0 ] || failwith "$1" "$2" "$3"
    echo "$res" | egrep "$3" > /dev/null
    [ "$?" -eq 0 ] || failwith "$1" "$2" "$3" "$res"
}

##### 24bit R-Instruction #####

### lw x0, -500(x1)
testentry 1 \
    ":reg: 0, 1F4 \
     :ram: 2A, 00 \
     :rom: 95, 10, 0C" \
    "x0=42"

### lb x0, -500(x1)
testentry 1 \
    ":reg: 0, 1F4 \
     :ram: FA \
     :rom: A5, 10, 0C" \
    "x0=65530"

### lbu x0, -500(x1)
testentry 1 \
    ":reg: 0, 1F4 \
     :ram: FA \
     :rom: 85, 10, 0C" \
    "x0=250"

### sw x0, -500(x1)
### lw x2, -500(x1)
testentry 2 \
    ":reg: 112A, 1F4, 0  \
     :rom: 9D, 10, 0C, 95, 12, 0C" \
    "x2=4394"

### sb x0, -500(x1)
### lbu x2, -500(x1)
testentry 2 \
    ":reg: 112A, 1F4, 0  \
     :rom: 8D, 10, 0C, 85, 12, 0C" \
    "x2=42"

### li x0, -500
testentry 1 \
    ":reg: 01  \
     :rom: B5, 00, 0C" \
    "x0=65036"

### add x0, x1, x2
testentry 1 \
    ":reg: 01, 02, 03  \
     :rom: 01, 10, 02" \
    "x0=5"

### sub x0, x1, x2
testentry 1 \
    ":reg: 01, 04, 02  \
     :rom: 09, 10, 02" \
    "x0=2"

### and x0, x1, x2
testentry 1 \
    ":reg: 01, 05, 03  \
     :rom: 11, 10, 02" \
    "x0=1"

### xor x0, x1, x2
testentry 1 \
    ":reg: 01, 04, 05  \
     :rom: 19, 10, 02" \
    "x0=1"

### or x0, x1, x2
testentry 1 \
    ":reg: 01, 05, 03  \
     :rom: 21, 10, 02" \
    "x0=7"

### lsl x0, x1, x2
testentry 1 \
    ":reg: 01, 05, 03  \
     :rom: 29, 10, 02" \
    "x0=40"

### lsr x0, x1, x2
testentry 1 \
    ":reg: 01, 15, 03  \
     :rom: 31, 10, 02" \
    "x0=2"

### asr x0, x1, x2
testentry 1 \
    ":reg: 01, FFF0, 04  \
     :rom: 39, 10, 02" \
    "x0=65535"


##### 24bit I-Instruction #####

### addi x0, x1, -10
testentry 1 \
    ":reg: 01, 07  \
     :rom: C3, 10, F6" \
    "x0=65533"

### addi x0, x1, 10
testentry 1 \
    ":reg: 01, 07  \
     :rom: C3, 10, 0A" \
    "x0=17"

### andi x0, x1, 10
testentry 1 \
    ":reg: 01, 08  \
     :rom: 53, 10, 0A" \
    "x0=8"

### xori x0, x1, 10
testentry 1 \
    ":reg: 01, 08  \
     :rom: 5B, 10, 0A" \
    "x0=2"

### ori x0, x1, 10
testentry 1 \
    ":reg: 01, 0C  \
     :rom: 63, 10, 0A" \
    "x0=14"

### lsli x0, x1, 3
testentry 1 \
    ":reg: 01, 05  \
     :rom: 2B, 10, 03" \
    "x0=40"

### lsri x0, x1, 3
testentry 1 \
    ":reg: 01, 15  \
     :rom: 33, 10, 03" \
    "x0=2"

### asri x0, x1, 4
testentry 1 \
    ":reg: 01, FFF0  \
     :rom: 3B, 10, 04" \
    "x0=65535"

##### 24bit J-Instruction #####

### beq x0, x1, 16
testentry 1 \
    ":reg: 01, FFF0  \
     :rom: 0F, 01, 10" \
    "pc=3"

### beq x0, x1, 16
testentry 1 \
    ":reg: 01, 01  \
     :rom: 0F, 01, 10" \
    "pc=16"

### bne x0, x1, 16
testentry 1 \
    ":reg: 01, FFF0  \
     :rom: 2F, 01, 10" \
    "pc=16"

### bne x0, x1, 16
testentry 1 \
    ":reg: 01, 01  \
     :rom: 2F, 01, 10" \
    "pc=3"

### blt x0, x1, 16
testentry 1 \
    ":reg: FFF0, 10 \
     :rom: 37, 01, 10" \
    "pc=16"

### blt x0, x1, 16
testentry 1 \
    ":reg: 10, FFF0 \
     :rom: 37, 01, 10" \
    "pc=3"

### blt x0, x1, 16
testentry 1 \
    ":reg: 01, 01  \
     :rom: 37, 01, 10" \
    "pc=3"

### bltu x0, x1, 16
testentry 1 \
    ":reg: FFF0, 10 \
     :rom: 17, 01, 10" \
    "pc=3"

### bltu x0, x1, 16
testentry 1 \
    ":reg: 10, FFF0 \
     :rom: 17, 01, 10" \
    "pc=16"

### bltu x0, x1, 16
testentry 1 \
    ":reg: 01, 01  \
     :rom: 17, 01, 10" \
    "pc=3"

### ble x0, x1, 16
testentry 1 \
    ":reg: FFF0, 10 \
     :rom: 3F, 01, 10" \
    "pc=16"

### ble x0, x1, 16
testentry 1 \
    ":reg: 10, FFF0 \
     :rom: 3F, 01, 10" \
    "pc=3"

### ble x0, x1, 16
testentry 1 \
    ":reg: 01, 01  \
     :rom: 3F, 01, 10" \
    "pc=16"

### bleu x0, x1, 16
testentry 1 \
    ":reg: FFF0, 10 \
     :rom: 1F, 01, 10" \
    "pc=3"

### bleu x0, x1, 16
testentry 1 \
    ":reg: 10, FFF0 \
     :rom: 1F, 01, 10" \
    "pc=16"

### bleu x0, x1, 16
testentry 1 \
    ":reg: 01, 01  \
     :rom: 1F, 01, 10" \
    "pc=16"

##### 16bit M-Instruction #####

### lwsp x0, 32(sp)
testentry 1 \
    ":reg: 0, 0 \
     :ram: 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2A,0 \
     :rom: 9C, 00" \
    "x0=42.+pc=2"

### swsp x0, 32(sp)
### lwsp x2, 32(sp)
testentry 2 \
    ":reg: 2A, 0, 0 \
     :ram: 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 \
     :rom: 84, 00, 9C, 02" \
    "x2=42.+pc=4"

### lsi x0, -32
testentry 1 \
    ":reg: 0, 0 \
     :rom: 8C, 00" \
    "x0=65504.+pc=2"

### lui x0, 32
testentry 1 \
    ":reg: 0, 0 \
     :rom: AC, 00" \
    "x0=32768.+pc=2"

### li x0, -1
### lui x0, 63
testentry 2 \
    ":reg: 0, 0 \
     :rom: F5, 00, FF, EC, F0" \
    "x0=65535.+pc=5"

##### 16bit R-Instruction #####

### mov x0, x1
testentry 1 \
    ":reg: 01, 02  \
     :rom: C0, 10" \
    "x0=2"

### add2 x0, x1
testentry 1 \
    ":reg: 01, 02  \
     :rom: 80, 10" \
    "x0=3"

### sub2 x0, x1
testentry 1 \
    ":reg: 03, 02  \
     :rom: 88, 10" \
    "x0=1"

### and2 x0, x1
testentry 1 \
    ":reg: 01, 03  \
     :rom: 90, 10" \
    "x0=1"

### xor2 x0, x1
testentry 1 \
    ":reg: 01, 03  \
     :rom: 98, 10" \
    "x0=2"

### or2 x0, x1
testentry 1 \
    ":reg: 04, 03  \
     :rom: A0, 10" \
    "x0=7"

### or2 x0, x1
testentry 1 \
    ":reg: 04, 03  \
     :rom: A0, 10" \
    "x0=7"

### lsl2 x0, x1
testentry 1 \
    ":reg: 13, 03  \
     :rom: A8, 10" \
    "x0=152"

### lsr2 x0, x1
testentry 1 \
    ":reg: 13, 03  \
     :rom: B0, 10" \
    "x0=2"

### asr2 x0, x1
testentry 1 \
    ":reg: fff0, 03  \
     :rom: B8, 10" \
    "x0=65534"

### lsli x0, 3
testentry 1 \
    ":reg: 05  \
     :rom: 2A, 30" \
    "x0=40"

### lsri x0, 3
testentry 1 \
    ":reg: 15  \
     :rom: 32, 30" \
    "x0=2"

### addi2 x0, -32
testentry 1 \
    ":reg: 10, 03  \
     :rom: 82, 00" \
    "x0=65520"

### andi2 x0, 26
testentry 1 \
    ":reg: 13  \
     :rom: 52, A0" \
    "x0=18"

echo "ok"
