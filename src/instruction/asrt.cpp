#include "simulator.hpp"
#include "util.hpp"

Simulator::State Simulator::asrt(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto rs = bitset(inst, 6, 11);
    auto expected = static_cast<int32_t>(signExt(bitset(inst, 11, 32), 21));

    if (m_state_iter->reg.at(rs) != expected) {
        addstr("Assertion failed.\n");
        printw("$r%-2d expected ", rs);
        printBitset(expected);
        addstr("\n     actually ");
        printBitset(m_state_iter->reg.at(static_cast<uint32_t>(rs)));
        refresh();
        getch();

        std::exit(1);
    }

    new_state.pc += 4;

    return new_state;
}
