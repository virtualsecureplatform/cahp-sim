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
    res=$(./main -q -t "$2" -d "$3" "$1")
    echo "$res" | grep "$4" > /dev/null
    [ "$?" -eq 0 ] || failwith "$1" "$2" "$3" "$4" "$res"
}

# # To make a test case;
# $ bin/llvm-objcopy -O binary -j .text foo.exe fo

# testentry #cycles ROM RAM expected

###
###   Just return 42 via x8.
###
###0000000000000000 _start:
###       0:	01 78 fe 01 	li	sp, 510
###       4:	00 73 06 00 	jal	6
###       8:	00 52 fe ff 	j	-2
###
###000000000000000c main:
###       c:	08 78 2a 00 	li	a0, 42
###      10:	00 40 	jr	ra
testentry 100 \
    "01 78 fe 01 00 73 06 00 00 52 fe ff 08 78 2a 00 00 40" \
    "" \
    "x8=42"

###
###   If 32767 < 0 then goto end.
###
###0000000000000000 _start:
###       0:	0e 78 ff 7f 	li	t0, 32767
###       4:	0f 78 00 80 	li	t1, 0
###       8:	fe c3 	cmp	t0, t1
###       a:	03 44 	jl	6
###       c:	08 78 2a 00 	li	a0, 42
###
###0000000000000010 end:
###      10:	00 52 fe ff 	j	-2
testentry 100 \
    "0e 78 ff 7f 0f 78 00 00 fe c3 03 44 08 78 2a 00 00 52 fe ff" \
    "" \
    "x8=42"

###
###   If 2 < 0 then goto loop
###
###0000000000000000 _start:
###       0:	08 78 02 00 	li	a0, 2
###       4:	09 78 00 00 	li	a1, 0
###
###0000000000000008 loop:
###       8:	98 c3 	cmp	a0, a1
###       a:	7f 46 	jb	-2
###       c:	08 78 2a 00 	li	a0, 42
###      10:	00 52 fe ff 	j	-2
testentry 100 \
    "08 78 02 00 09 78 00 00 98 c3 7f 46 08 78 2a 00 00 52 fe ff" \
    "" \
    "x8=42"

###
###   If 32767 < -32768 then goto end.
###
###0000000000000000 _start:
###       0:	0e 78 ff 7f 	li	t0, 32767
###       4:	0f 78 00 80 	li	t1, -32768
###       8:	fe c3 	cmp	t0, t1
###       a:	03 44 	jl	6
###       c:	08 78 2a 00 	li	a0, 42
###
###0000000000000010 end:
###      10:	00 52 fe ff 	j	-2
testentry 100 \
    "0e 78 ff 7f 0f 78 80 00 fe c3 03 44 08 78 2a 00 00 52 fe ff" \
    "" \
    "x8=42"

###
###   xor 255, 255
###
###0000000000000000 _start:
###       0:	08 78 ff 00 	li	a0, 255
###       4:	09 78 ff 00 	li	a1, 255
###       8:	98 e6 	xor	a0, a1
testentry 10 \
    "08 78 ff 00 09 78 ff 00 98 e6" \
    "" \
    "x8=0"

echo "ok"
