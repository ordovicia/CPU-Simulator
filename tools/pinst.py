# -*- coding: utf-8 -*-

import sys
from gen_instruction import insts

fn = sys.argv[1] if len(sys.argv) > 1 else 'instruction.log'

counts = {}
with open(fn) as f:
    for l in f:
        if l[0] == '#':
            continue
        inst, cnt = [int(x) for x in l.split()]
        counts[inst] = cnt

for inst in range(64):
    if inst in insts:
        name = insts[inst][0]
        cnt = counts.get(inst, 0)
        print(name, cnt)
