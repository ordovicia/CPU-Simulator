#include "simulator.hpp"
#include "util.hpp"

Simulator::PreState Simulator::asrt_s(Instruction inst)
{
    auto rs = bitset(inst, 6, 11);
    auto reg = ftou(m_freg.at(rs));
    auto expected = m_codes.at(m_pc / 4 + 1);

    if (reg != expected) {
        addstr("Assertion failed.\n");
        printw("$f%-2d expected ", rs);
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
