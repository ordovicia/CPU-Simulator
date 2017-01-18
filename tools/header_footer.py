# -*- coding: utf-8 -*-

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

inst_cpp_header = '''#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::execInst(OpCode opcode, Instruction inst)
{
    switch (opcode) {
'''

inst_cpp_footer = '''    default:
        FAIL("# Error. No such instruction");
    }
}
'''

disasm_header = '''#include "simulator.hpp"

void Simulator::initDisassembler()
{
    using Field = Mnemonic::OperandField;

'''

test_run_header = '''#!/bin/sh

set -e

insts=(
'''

test_run_footer = '''
)

root=$(git rev-parse --show-toplevel)
cd $root/test
testdir=$PWD

function do_test() {
    inst=$1
    cd $testdir
    if [ -e $inst ]; then
        cd $inst
        python $root/tools/ascii2bin.py $inst.txt
        echo "testing" $inst "..."
        $root/build/simulator -f $inst.bin -i $testdir/input.txt -r
        echo "passed"
    fi
}

if [ $# -ge 1 ]; then
    do_test $*
else
    for inst in ${insts[@]}; do
        do_test $inst
    done
fi
'''
