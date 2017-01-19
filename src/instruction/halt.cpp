#include <ncurses.h>
#include "simulator.hpp"

Simulator::PreState Simulator::halt(Instruction /* inst */)
{
    m_halt = true;
    m_running = false;

    dumpLog();

    return makePrePCState(m_pc);
}
