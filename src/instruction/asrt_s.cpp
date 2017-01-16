#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::asrt_s(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto rs = bitset(inst, 6, 11);
    auto reg = ftou(m_state_iter->freg.at(rs));
    auto expected = m_codes.at(m_state_iter->pc / 4 + 1);

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

    new_state.pc += 8;

    return new_state;
}
