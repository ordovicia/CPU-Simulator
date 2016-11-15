#include "simulator.hpp"
#include "util.hpp"
#include <ncurses.h>

Simulator::State Simulator::asrt(Instruction inst)
{
    auto new_state = *m_state_iter;
    new_state.memory_patch = MemoryPatch{};

    auto op = decodeI(inst);
    auto expected = static_cast<int32_t>(signExt(op.immediate, 16));

    if (m_state_iter->reg.at(op.rs) != expected) {
        addstr("Assertion failed.\n");
        printw("$r%-2d expected ", op.rs);
        printBitset(expected);
        addstr("\n     actually ");
        printBitset(m_state_iter->reg.at(static_cast<uint32_t>(op.rs)));
        refresh();
        getch();

        endwin();
        std::exit(1);
    }

    new_state.pc += 4;

    return new_state;
}
