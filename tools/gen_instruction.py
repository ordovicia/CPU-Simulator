#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import os
import subprocess
import shutil

insts = {
    4: ('NOP', 'N'),
    5: ('HALT', 'N'),
    6: ('IN', 'R'),
    7: ('OUT', 'R'),

    8: ('ADD', 'R'),
    9: ('ADDI', 'I'),
    10: ('SUB', 'R'),
    11: ('LUI', 'I'),

    12: ('DIV', 'R'),
    13: ('MULT', 'R'),

    16: ('SLL', 'R'),
    17: ('SRA', 'R'),
    18: ('SRL', 'R'),
    20: ('AND_', 'R'),
    21: ('ANDI', 'I'),
    22: ('OR_', 'R'),
    23: ('ORI', 'I'),
    24: ('XOR_', 'R'),
    25: ('XORI', 'I'),
    26: ('NOR', 'R'),

    28: ('LW', 'I'),
    29: ('LWO', 'R'),
    30: ('SW', 'I'),
    31: ('SWO', 'R'),

    32: ('BEQ', 'I'),
    33: ('BGEZ', 'I'),
    34: ('BGTZ', 'I'),
    35: ('BLEZ', 'I'),
    36: ('BLTZ', 'I'),
    37: ('BGEZAL', 'I'),
    38: ('BLTZAL', 'I'),
    39: ('J', 'J'),
    40: ('JAL', 'J'),
    41: ('JR', 'I'),
    42: ('JALR', 'I'),

    48: ('LWC1', 'I'),
    49: ('LWOC1', 'R'),
    50: ('SWC1', 'I'),
    51: ('SWOC1', 'R'),
    52: ('MTC1', 'I'),
    53: ('MFC1', 'I'),

    54: ('ABS_S', 'I'),
    55: ('NEG_S', 'I'),
    56: ('ADD_S', 'I'),
    57: ('SUB_S', 'I'),
    58: ('MUL_S', 'I'),
    59: ('DIV_S', 'I'),
    60: ('CVT_S_W', 'I'),
    61: ('CVT_W_S', 'I'),
    62: ('MOV_S', 'I'),

    63: ('ASRT', 'I'),
}

opcode_name = 'opcode.hpp'
inst_hpp_name = 'instruction.hpp'
inst_cpp_name = 'init_inst.cpp'
disasm_name = 'init_disasm.cpp'
test_run_name = 'run.sh'

opcode_header = '''#pragma once

#include <cstdint>
#include <functional>

enum class OpCode : uint32_t {
'''
opcode_footer = '''};

namespace std
{
    template <>
    struct hash<OpCode> {
        size_t operator()(const OpCode& op) const
        {
            return hash<uint32_t>{}(static_cast<uint32_t>(op));
        }
    };
}
'''

cpp_header = '''#include "simulator.hpp"

void Simulator::initInstruction()
{
'''

disasm_header = '''#include "simulator.hpp"

void Simulator::initDisassembler()
{
'''

test_run_header = '''#!/bin/sh

set -e

testdir=$PWD
cd ..
root=$PWD

insts=(
'''

test_run_footer = '''
)

for inst in ${insts[@]}; do
    cd $testdir
    if [ -e $inst ]; then
        cd $inst
        python $root/tools/ascii2bin.py $inst.txt ${inst%.txt}.bin
        echo "testing" $inst "..."
        $root/build/simulator -f $inst.bin -i $testdir/input.txt -r
        echo "passed"
    fi
done
'''


def main():
    # opcode.hpp
    with open(opcode_name + '.tmp', 'w') as opcode_tmp:
        opcode_tmp.write(opcode_header)
        for (n, c) in insts.items():
            opcode_tmp.write('    {} = {},\n'.format(c[0], n))
        opcode_tmp.write(opcode_footer)

    # instruction.hpp and init_inst.cpp
    with open(inst_hpp_name + '.tmp', 'w') as hpp_tmp:
        with open(inst_cpp_name + '.tmp', 'w') as cpp_tmp:
            cpp_tmp.write(cpp_header)
            for inst_ in insts.values():
                inst = inst_[0]
                hpp_tmp.write(
                    '    State {}(Instruction);\n'.format(inst.lower(), ))
                cpp_tmp.write('''    m_inst_funcs.emplace(
        OpCode::{}, [this](Instruction inst) {{ return {}(inst); }});\n'''
                              .format(inst, inst.lower()))
                cpp_tmp.write(
                    '    m_inst_cnt.emplace(OpCode::{}, 0);\n'.format(inst))
            cpp_tmp.write('}\n')

    # disassembler
    def mneumonic(m):
        m = m.lower()
        m = re.sub(r'_$', '', m)
        m = re.sub(r'_', '.', m)
        return m

    with open(disasm_name + '.tmp', 'w') as disasm_tmp:
        disasm_tmp.write(disasm_header)
        for (n, c) in insts.items():
            disasm_tmp.write(
                '''    m_mnemonic_table.emplace(
        OpCode::{}, std::make_pair("{}", OperandType::{}));\n'''
                .format(c[0], mneumonic(c[0]), c[1]))
        disasm_tmp.write('}\n')

    # tester
    with open(test_run_name, 'w') as run_tmp:
        run_tmp.write(test_run_header)
        for inst in insts.values():
            run_tmp.write(inst[0].lower())
            run_tmp.write(' ')
        run_tmp.write(test_run_footer)

    # Detect diff and move/remove
    def diff(n):
        return (not os.path.exists(n) or subprocess.call(['diff', n, n + '.tmp']))

    def mv(n):
        shutil.move(n + '.tmp', n)

    def rm(n):
        os.remove(n + '.tmp')

    names = [opcode_name, inst_hpp_name, inst_cpp_name, disasm_name]
    if any(diff(n) for n in names):
        for n in names:
            mv(n)
    else:
        for n in names:
            rm(n)
    os.chmod(test_run_name, 0o755)
    shutil.move(test_run_name, '../test/' + test_run_name)


if __name__ == '__main__':
    main()
