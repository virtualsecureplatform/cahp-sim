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
    echo "$res" | grep "$4" > /dev/null
    [ "$?" -eq 0 ] || failwith "$1" "$2" "$3" "$res"
}

### add x0, x1, x2
testentry 1 \
    ":reg: 01, 02, 03  \
     :rom: 01, 10, 02" \
    "x0=3"

echo "ok"
