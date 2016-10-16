import os, subprocess, shutil

insts = {
    0: 'NOP',
    1: 'ADD',
    2: 'ADDI',
    3: 'SUB',
    60: 'HALT',
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
    # 33: 'J',
    # 34: 'JAL',
    # 35: 'JR',
    # 36: 'JALR',
}

opcode_name = 'opcode.hpp'
hpp_name = 'instruction.hpp'
cpp_name = 'init_inst.cpp'

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

# opcode.hpp
with open(opcode_name + '.tmp', 'w') as opcode_tmp:
    opcode_tmp.write(opcode_header)
    for (n, c) in insts.items():
        opcode_tmp.write('    {} = {},\n'.format(c, n))
    opcode_tmp.write(opcode_footer)

# Detect diff
if not os.path.exists(opcode_name) or subprocess.call(['diff', opcode_name, opcode_name + '.tmp']):
    mv = True
else:
    mv = False

# instruction.hpp and init_inst.cpp
with open(hpp_name + '.tmp', 'w') as hpp_tmp:
    with open(cpp_name + '.tmp', 'w') as cpp_tmp:
        cpp_tmp.write(cpp_header)
        for inst in insts.values():
            hpp_tmp.write('    State {}(Instruction, StateIter);\n'.format(inst.lower(), ))
            cpp_tmp.write('    m_inst_funcs.emplace(OpCode::{}, [this](Instruction inst, StateIter si) {{ return {}(inst, si); }});\n'.format(inst, inst.lower()))
            cpp_tmp.write('    m_inst_cnt.emplace(OpCode::{}, 0);\n'.format(inst))
        cpp_tmp.write('}\n')

# Move if differed
if mv:
    shutil.move(opcode_name + '.tmp', opcode_name)
    shutil.move(hpp_name + '.tmp', hpp_name)
    shutil.move(cpp_name + '.tmp', cpp_name)
