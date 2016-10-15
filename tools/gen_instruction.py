import os, subprocess, shutil

insts = {
    0: 'NOP',
    1: 'ADD',
    2: 'ADDI',
    3: 'SUB',
    60: 'HALT',
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
