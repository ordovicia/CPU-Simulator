#include <sstream>
#include <iomanip>
#include "simulator.hpp"
#include "util.hpp"

std::string Simulator::disasm(Simulator::Instruction inst) const
{
    using Field = Mnemonic::OperandField;

    auto opcode = decodeOpCode(inst);
    if (m_mnemonic_table.count(opcode) == 0)
        return "";

    auto mnemo = m_mnemonic_table.at(opcode);

    std::ostringstream oss;
    oss << std::setw(7) << mnemo.mnemonic;

    auto writeR = [&](Field f, const int32_t& r) {
        switch (f) {
        case Field::R:
            oss << " r" << std::setw(2) << std::left << r;
            return;
        case Field::F:
            oss << " f" << std::setw(2) << std::left << r;
            return;
        default:
            oss << "  - ";
            return;
        }
    };

    const auto& of = mnemo.operand_field;
    switch (mnemo.type) {
    case OperandType::R: {
        auto op = decodeR(inst);
        writeR(of[0], op.rs);
        writeR(of[1], op.rt);
        writeR(of[2], op.rd);
        if (of[3] == Field::I)
            oss << ' ' << op.shamt;
        else
            oss << "  - ";
        break;
    }
    case OperandType::I: {
        auto op = decodeI(inst);
        writeR(of[0], op.rs);
        writeR(of[1], op.rt);

        if (of[2] == Field::I) {
            auto imm_ext = static_cast<int32_t>(signExt(op.immediate, 16));
            oss << ' ';
            if (imm_ext < 0)
                oss << imm_ext;
            else
                oss << op.immediate;
        } else {
            oss << "  - ";
        }
        break;
    }
    case OperandType::J: {
        auto op = decodeJ(inst);
        oss << ' ' << op.addr;
        break;
    }
    default:
        break;
    }

    return oss.str();
}
