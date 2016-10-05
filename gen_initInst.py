insts = [
    'NOP',
    'ADD',
    'ADDI',
    'SUB',
]

print('''#include "simulator.hpp"

void Simulator::initInst()
{''')

for inst in insts:
    print('    m_inst_funcs.emplace(OpCode::{}, [this](Instruction inst) {{ {}(inst); }});'.format(inst, inst.lower()))
    print('    m_inst_cnt.emplace(OpCode::{}, 0);'.format(inst))

print('}')
