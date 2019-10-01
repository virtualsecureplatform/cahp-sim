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
    res=$(./cahp-sim -t "$2" -d "$3" "$1")
    echo "$res" | grep "$4" > /dev/null
    [ "$?" -eq 0 ] || failwith "$1" "$2" "$3" "$4" "$res"
}

# # To make a test case;
# $ bin/llvm-objcopy -O binary -j .text foo.exe fo

# testentry #cycles ROM RAM expected

### add x0, x1, x2
testentry 1 \
    "01 10 02" \
    "" \
    "x8=0"

echo "ok"
