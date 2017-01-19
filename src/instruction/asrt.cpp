#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::asrt(Instruction inst)
{
    auto rs = bitset(inst, 6, 11);
    auto reg = static_cast<uint32_t>(m_reg.at(rs));
    auto expected = m_codes.at(m_pc / 4 + 1);

    if (reg != expected) {
        addstr("Assertion failed.\n");
        printw("$r%-2d expected ", rs);
        printBitset(expected);
        addstr("\n     actually ");
        printBitset(reg);
        refresh();
        getch();

        std::exit(1);
    }

    auto pre_state = makePrePCState(m_pc);
    m_pc += 8;

    return pre_state;
}
