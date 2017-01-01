#include <sstream>
#include <iomanip>
#include "simulator.hpp"
#include "util.hpp"

std::string Simulator::disasm(Simulator::Instruction inst)
{
    auto op = m_mnemonic_table.at(decodeOpCode(inst));

    std::ostringstream oss;
    oss << " | " << std::setw(7) << op.first << ' ';

    switch (op.second) {
    case OperandType::R: {
        auto opr = decodeR(inst);
        oss << 'r' << std::setw(2) << std::left << opr.rs
            << " r" << std::setw(2) << std::left << opr.rt
            << " r" << std::setw(2) << std::left << opr.rd
            << ' ' << opr.shamt;
        break;
    }
    case OperandType::I: {
        auto opr = decodeI(inst);
        auto imm_ext = static_cast<int32_t>(signExt(opr.immediate, 16));
        oss << 'r' << std::setw(2) << std::left << opr.rs
            << " r" << std::setw(2) << std::left << opr.rt << ' ';
        if (imm_ext < 0)
            oss << imm_ext;
        else
            oss << opr.immediate;
        break;
    }
    case OperandType::J: {
        auto opr = decodeJ(inst);
        oss << opr.addr;
        break;
    }
    default:
        break;
    }

    return oss.str();
}
