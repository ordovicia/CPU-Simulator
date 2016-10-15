import os, subprocess, shutil

insts = [
    'NOP',
    'ADD',
    'ADDI',
    'SUB',
    'HALT',
]

hpp_name = 'instruction.hpp'
cpp_name = 'init_inst.cpp'

hpp_tmp = open(hpp_name + '.tmp', 'w')
cpp_tmp = open(cpp_name + '.tmp', 'w')

cpp_tmp.write('''#include "simulator.hpp"

void Simulator::initInstruction()
{
''')
for inst in insts:
    hpp_tmp.write('    State {}(Instruction, StateIter);\n'.format(inst.lower(), ))
    cpp_tmp.write('    m_inst_funcs.emplace(OpCode::{}, [this](Instruction inst, StateIter si) {{ return {}(inst, si); }});\n'.format(inst, inst.lower()))
    cpp_tmp.write('    m_inst_cnt.emplace(OpCode::{}, 0);\n'.format(inst))
cpp_tmp.write('}\n')

hpp_tmp.close()
cpp_tmp.close()

mv = False
if not os.path.exists(hpp_name) or subprocess.call(['diff', hpp_name, hpp_name + '.tmp']):
    mv = True
if not os.path.exists(cpp_name) or subprocess.call(['diff', cpp_name, cpp_name + '.tmp']):
    mv = True
if mv:
    shutil.move(hpp_name + '.tmp', hpp_name)
    shutil.move(cpp_name + '.tmp', cpp_name)
