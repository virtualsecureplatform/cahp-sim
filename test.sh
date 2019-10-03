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
    echo "$res" | grep "$3" > /dev/null
    [ "$?" -eq 0 ] || failwith "$1" "$2" "$3" "$res"
}

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

echo "ok"
