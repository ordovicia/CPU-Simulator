import re
import os
import subprocess
import shutil

insts = {
    0: ('NOP', 'N'),
    1: ('ADD', 'R'),
    2: ('ADDI', 'I'),
    3: ('SUB', 'R'),
    4: ('LUI', 'I'),
    5: ('SLL', 'R'),
    7: ('SRA', 'R'),
    9: ('SRL', 'R'),
    11: ('AND_', 'R'),
    12: ('ANDI', 'I'),
    13: ('OR_', 'R'),
    14: ('ORI', 'I'),
    15: ('XOR_', 'R'),
    16: ('XORI', 'I'),
    17: ('NOR', 'R'),
    18: ('DIV', 'R'),
    20: ('MULT', 'R'),
    26: ('BEQ', 'I'),
    27: ('BGEZ', 'I'),
    28: ('BGTZ', 'I'),
    29: ('BLEZ', 'I'),
    30: ('BLTZ', 'I'),
    31: ('BGEZAL', 'I'),
    32: ('BLTZAL', 'I'),
    33: ('J', 'J'),
    34: ('JAL', 'J'),
    35: ('JR', 'R'),
    36: ('JALR', 'R'),
    39: ('LW', 'I'),
    41: ('SW', 'I'),
    42: ('ABS_S', 'R'),
    43: ('NEG_S', 'R'),
    44: ('ADD_S', 'R'),
    45: ('SUB_S', 'R'),
    46: ('MUL_S', 'R'),
    47: ('DIV_S', 'R'),
    48: ('CVT_S_W', 'R'),
    49: ('CVT_W_S', 'R'),
    50: ('MOV_S', 'R'),
    51: ('C_EQ_S', 'R'),
    52: ('C_LE_S', 'R'),
    53: ('C_LT_S', 'R'),
    54: ('BC1T', 'J'),
    55: ('BC1F', 'J'),
    56: ('LWC1', 'I'),
    57: ('SWC1', 'I'),
    58: ('MTC1', 'R'),
    59: ('MFC1', 'R'),
    60: ('HALT', 'N'),
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
        $root/build/simulator -f $inst.bin -r
    fi
done
'''

# opcode.hpp
with open(opcode_name + '.tmp', 'w') as opcode_tmp:
    opcode_tmp.write(opcode_header)
    for (n, c) in insts.items():
        opcode_tmp.write('    {} = {},\n'.format(c[0], n))
    opcode_tmp.write(opcode_footer)

# instruction.hpp and init_inst.cpp
with open(hpp_name + '.tmp', 'w') as hpp_tmp:
    with open(cpp_name + '.tmp', 'w') as cpp_tmp:
        cpp_tmp.write(cpp_header)
        for inst_ in insts.values():
            inst = inst_[0]
            hpp_tmp.write(
                '    State {}(Instruction);\n'.format(inst.lower(), ))
            cpp_tmp.write('    m_inst_funcs.emplace(OpCode::{}, [this](Instruction inst) {{ return {}(inst); }});\n'
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
            '    m_mnemonic_table.emplace(OpCode::{}, std::make_pair("{}", OperandType::{}));\n'
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

names = [opcode_name, hpp_name, cpp_name, disasm_name]
if any(diff(n) for n in names):
    for n in names:
        mv(n)
else:
    for n in names:
        rm(n)
os.chmod(test_run_name, 0o755)
shutil.move(test_run_name, '../test/' + test_run_name)
