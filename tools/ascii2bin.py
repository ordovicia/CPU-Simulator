# -*- coding:utf-8 -*-

"""
Ascii -> Binary converter

Usage: ascii2bin.py [input ascii file] [output binary file]

0, 1のasciiで書かれたテキストファイルをバイナリに変換します。
入力するテキストファイルは、一行に32文字の0または1が書かれたものです。
半角スペース、および'#'以降の文字は行末まで無視されます。

Example input:
    000000 00000 00000 00000 00000 000000   # nop
    000010 00000 00001 0000000011111111     # addi %r0 %r1 0xff
    000001 00001 00010 00010 00000 000000   # add  %r1 %r2 %r2
    000001 00010 00001 00001 00000 000000   # add  %r2 %r1 %r1
    000001 00001 00010 00010 00000 000000   # add  %r1 %r2 %r2
    000001 00010 00001 00001 00000 000000   # add  %r2 %r1 %r1
    000001 00001 00010 00010 00000 000000   # add  %r1 %r2 %r2
    000001 00010 00001 00001 00000 000000   # add  %r2 %r1 %r1
    000001 00001 00010 00010 00000 000000   # add  %r1 %r2 %r2
    000001 00010 00001 00001 00000 000000   # add  %r2 %r1 %r1
    111100 00000 00000 00000 00000 000000   # halt
"""

import sys, struct

if len(sys.argv) != 3:
    print("Usage: {} [input ascii file] [output binary file]".format(sys.argv[0], ))
    exit(1)

fin = open(sys.argv[1], "r")
fout = open(sys.argv[2], "wb")

def invalid_line(i, l):
    print("Invalid line\n{}: {}".format(i, l))
    exit(1)

for (i, l) in enumerate(fin.readlines()):
    code = 0
    cnt = 0
    for c in l:
        if c == '0':
            code <<= 1
        elif c == '1':
            code <<= 1
            code |= 1
        elif c == '#' or c == '\n':
            break
        elif c == ' ':
            continue
        else:
            invalid_line(i, l)
        # print(c, end = "")
        cnt += 1
    if cnt != 32:
        invalid_line(i, l)
    # print("")
    fout.write(struct.pack('I', code))
