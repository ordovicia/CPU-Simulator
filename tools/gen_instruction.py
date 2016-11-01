import re
import os
import subprocess
import shutil

insts = {
    0: 'NOP',
    1: 'ADD',
    2: 'ADDI',
    3: 'SUB',
    4: 'LUI',
    5: 'SLL',
    6: 'SLLV',
    7: 'SRA',
    8: 'SRAV',
    9: 'SRL',
    10: 'SRLV',
    11: 'AND_',
    12: 'ANDI',
    13: 'OR_',
    14: 'ORI',
    15: 'XOR_',
    16: 'XORI',
    17: 'NOR',
    18: 'DIV',
    19: 'DIVU',
    20: 'MULT',
    21: 'MULTU',
    22: 'MFHI',
    23: 'MFLO',
    24: 'MTHI',
    25: 'MTLO',
    26: 'BEQ',
    27: 'BGEZ',
    28: 'BGTZ',
    29: 'BLEZ',
    30: 'BLTZ',
    31: 'BGEZAL',
    32: 'BLTZAL',
    33: 'J',
    34: 'JAL',
    35: 'JR',
    36: 'JALR',
    37: 'LB',
    38: 'LBU',
    39: 'LW',
    40: 'SB',
    41: 'SW',
    42: 'ABS_S',
    43: 'NEG_S',
    44: 'ADD_S',
    45: 'SUB_S',
    46: 'MUL_S',
    47: 'DIV_S',
    48: 'CVT_S_W',
    49: 'CVT_W_S',
    50: 'MOV_S',
    51: 'C_EQ_S',
    52: 'C_LE_S',
    53: 'C_LT_S',
    54: 'BC1T',
    55: 'BC1F',
    56: 'LWC1',
    57:	'SWC1',
    58:	'MTC1',
    59:	'MFC1',
    60: 'HALT',
}

opcode_name = 'opcode.hpp'
hpp_name = 'instruction.hpp'
cpp_name = 'init_inst.cpp'
disasm_name = 'init_disasm.cpp'
test_run_name = 'run.sh'

opcode_header = '''#pragma once

#include <cstdint>
#include <functional>

enum class OpCode : uint32_t {
'''
opcode_footer = '''};

struct OpCodeHash {
    size_t operator()(OpCode op) const noexcept
    {
        return std::hash<uint32_t>{}(static_cast<uint32_t>(op));
    }
};
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
        $root/build/simulator -f $inst.bin -r
    fi
done
'''

# opcode.hpp
with open(opcode_name + '.tmp', 'w') as opcode_tmp:
    opcode_tmp.write(opcode_header)
    for (n, c) in insts.items():
        opcode_tmp.write('    {} = {},\n'.format(c, n))
    opcode_tmp.write(opcode_footer)

# instruction.hpp and init_inst.cpp
with open(hpp_name + '.tmp', 'w') as hpp_tmp:
    with open(cpp_name + '.tmp', 'w') as cpp_tmp:
        cpp_tmp.write(cpp_header)
        for inst in insts.values():
            hpp_tmp.write(
                '    State {}(Instruction);\n'.format(inst.lower(), ))
            cpp_tmp.write('    m_inst_funcs.emplace(OpCode::{}, [this](Instruction inst) {{ return {}(inst); }});\n'.format(
                inst, inst.lower()))
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
            '    m_mnemonic_table.emplace(OpCode::{}, "{}");\n'.format(c, mneumonic(c)))
    disasm_tmp.write('}\n')

# tester
with open(test_run_name, 'w') as run_tmp:
    run_tmp.write(test_run_header)
    for inst in insts.values():
        run_tmp.write(inst.lower())
        run_tmp.write(' ')
    run_tmp.write(test_run_footer)


# Detect diff and move/remove
def diff(n):
    return (not os.path.exists(n) or subprocess.call(['diff', n, n + '.tmp']))


def mv(n):
    shutil.move(n + '.tmp', n)


def rm(n):
    os.remove(n + '.tmp')

names = [opcode_name, hpp_name, cpp_name, disasm_name]
if any(diff(n) for n in names):
    for n in names:
        mv(n)
else:
    for n in names:
        rm(n)
os.chmod(test_run_name, 0o755)
shutil.move(test_run_name, '../test/' + test_run_name)
