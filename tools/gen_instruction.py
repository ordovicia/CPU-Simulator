#!/usr/bin/env python
# -*- coding: utf-8 -*-

import re
import os
import subprocess
import shutil
from header_footer import *

insts = {
    # opcode: (mnemonic, operand type, [used operand field])
    0: ('ASRT', 'I', ['R']),
    1: ('ASRT_S', 'I', ['F']),

    4: ('NOP', 'N', []),
    5: ('HALT', 'N', []),
    6: ('IN', 'R', [None, None, 'R']),
    7: ('OUT', 'R', ['R']),

    8: ('ADD', 'R', ['R', 'R', 'R']),
    9: ('ADDI', 'I', ['R', 'R', 'I']),
    10: ('SUB', 'R', ['R', 'R', 'R']),
    11: ('LUI', 'I', [None, 'R', 'I']),

    12: ('DIV', 'R', ['R', 'R', 'R']),
    13: ('MULT', 'R', ['R', 'R', 'R']),
    14: ('DIVI', 'I', ['R', 'R', 'I']),
    15: ('MULTI', 'I', ['R', 'R', 'I']),

    16: ('SLL', 'R', ['R', None, 'R', 'I']),
    17: ('SRA', 'R', ['R', None, 'R', 'I']),
    18: ('SRL', 'R', ['R', None, 'R', 'I']),
    20: ('AND_', 'R', ['R', 'R', 'R']),
    21: ('ANDI', 'I', ['R', 'R', 'I']),
    22: ('OR_', 'R', ['R', 'R', 'R']),
    23: ('ORI', 'I', ['R', 'R', 'I']),
    24: ('XOR_', 'R', ['R', 'R', 'R']),
    25: ('XORI', 'I', ['R', 'R', 'I']),
    26: ('NOR', 'R', ['R', 'R', 'R']),

    28: ('LW', 'I', ['R', 'R', 'I']),
    29: ('LWO', 'R', ['R', 'R', 'R']),
    30: ('SW', 'I', ['R', 'R', 'I']),
    31: ('SWO', 'R', ['R', 'R', 'R']),

    32: ('BEQ', 'I', ['R', 'R', 'I']),
    33: ('BGEZ', 'I', ['R', None, 'I']),
    34: ('BGTZ', 'I', ['R', None, 'I']),
    35: ('BLEZ', 'I', ['R', None, 'I']),
    36: ('BLTZ', 'I', ['R', None, 'I']),
    37: ('BGEZAL', 'I', ['R', None, 'I']),
    38: ('BLTZAL', 'I', ['R', None, 'I']),
    39: ('J', 'J', ['I']),
    40: ('JAL', 'J', ['I']),
    41: ('JR', 'I', ['R']),
    42: ('JALR', 'I', ['R', 'R']),

    48: ('LWC1', 'I', ['R', 'F', 'I']),
    49: ('LWOC1', 'R', ['R', 'F', 'F']),
    50: ('SWC1', 'I', ['F', 'R', 'I']),
    51: ('SWOC1', 'R', ['F', 'R', 'R']),
    52: ('MTC1', 'I', ['R', 'F']),
    53: ('MFC1', 'I', ['R', 'F']),

    54: ('ABS_S', 'I', ['F', 'F']),
    55: ('NEG_S', 'I', ['F', 'F']),
    56: ('ADD_S', 'I', ['F', 'F', 'F']),
    57: ('SUB_S', 'I', ['F', 'F', 'F']),
    58: ('MUL_S', 'I', ['F', 'F', 'F']),
    59: ('DIV_S', 'I', ['F', 'F', 'F']),
    60: ('CVT_S_W', 'I', ['F', 'F']),
    61: ('CVT_W_S', 'I', ['F', 'F']),
    62: ('MOV_S', 'I', ['F', 'F']),
}

opcode_name = 'opcode.hpp'
inst_hpp_name = 'instructions.hpp'
inst_cpp_name = 'exec_inst.cpp'
disasm_name = 'init_disasm.cpp'
test_run_name = 'run.sh'


def mnemonic(m):
    m = m.lower()
    m = re.sub(r'_$', '', m)
    m = re.sub(r'_', '.', m)
    return m


def main():
    # opcode
    with open(opcode_name + '.tmp', 'w') as opcode_tmp:
        opcode_tmp.write(opcode_header)
        for (n, c) in insts.items():
            opcode_tmp.write('    {} = {},\n'.format(c[0], n))
        opcode_tmp.write(opcode_footer)

    # instructions
    with open(inst_hpp_name + '.tmp', 'w') as inst_hpp_tmp:
        with open(inst_cpp_name + '.tmp', 'w') as inst_cpp_tmp:
            inst_cpp_tmp.write(inst_cpp_header)
            for inst_ in insts.values():
                inst = inst_[0]
                inst_hpp_tmp.write(
                    '    PreState {}(Instruction);\n'.format(inst.lower(), ))
                inst_cpp_tmp.write('''    case OpCode::{}:
        return {}(inst);\n'''.format(inst, inst.lower()))
            inst_cpp_tmp.write(inst_cpp_footer)

    # disassembler
    def operand_field(of):
        r = '{'
        for i in range(4):
            r += 'Field::' \
                + (of[i] if (i < len(of) and (of[i] is not None)) else 'N') \
                + (', ' if i != 3 else '')
        return r + '}'

    with open(disasm_name + '.tmp', 'w') as disasm_tmp:
        disasm_tmp.write(disasm_header)
        for (n, c) in insts.items():
            disasm_tmp.write(
                '''    m_mnemonic_table.emplace(
        OpCode::{},
        Mnemonic{{"{}", OperandType::{},
            {}}});\n'''
                .format(c[0], mnemonic(c[0]), c[1], operand_field(c[2])))
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
        return (not os.path.exists(n)
                or subprocess.call(['diff', n, n + '.tmp']))

    def mv(n):
        shutil.move(n + '.tmp', n)

    def rm(n):
        os.remove(n + '.tmp')

    names = [opcode_name, inst_hpp_name, inst_cpp_name, disasm_name]
    if any([diff(n) for n in names]):
        for n in names:
            mv(n)
    else:
        for n in names:
            rm(n)
    os.chmod(test_run_name, 0o755)
    shutil.move(test_run_name, '../test/' + test_run_name)


if __name__ == '__main__':
    main()
