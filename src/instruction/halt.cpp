#include <ncurses.h>
#include "simulator.hpp"

Simulator::State Simulator::halt(Instruction /* inst */)
{
    m_halt = true;
    m_running = false;

    dumpLog();

    return *m_state_iter;
}
