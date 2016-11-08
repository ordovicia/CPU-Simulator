#include <ncurses.h>
#include "simulator.hpp"
#include "util.hpp"

std::string Simulator::disasm(Simulator::Instruction inst)
{
    auto op = m_mnemonic_table.at(decodeOpCode(inst));
    auto mnemonic = op.first.c_str();

    switch (op.second) {
    case OperandType::R: {
        auto opr = decodeR(inst);
        printw(" | %7s %d %d %d %d\n",
            mnemonic, opr.rs, opr.rt, opr.rd, opr.shamt);
        break;
    }
    case OperandType::I: {
        auto opr = decodeI(inst);
        printw(" | %7s %d %d %d (%d)\n",
            mnemonic, opr.rs, opr.rt, opr.immediate,
            static_cast<int32_t>(signExt(opr.immediate, 16)));
        break;
    }
    case OperandType::J: {
        auto opr = decodeJ(inst);
        printw(" | %7s %d\n", mnemonic, opr.addr);
        break;
    }
    default:
        addstr("\n");
        break;
    }

    return mnemonic;
}
