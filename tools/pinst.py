# -*- coding: utf-8 -*-

import sys
from gen_instruction import insts

fn = sys.argv[1] if len(sys.argv) > 1 else 'instruction.log'

with open(fn) as f:
    ls = [ls.split() for ls in f.readlines()]
    ls = {int(inst): int(n) for (inst, n) in ls}
    for i in range(64):
        if i in ls:
            print(insts[i][0], ls[i])
