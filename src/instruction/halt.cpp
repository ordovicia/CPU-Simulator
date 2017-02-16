#include <ncurses.h>
#include "simulator.hpp"

Simulator::PreState Simulator::halt(Instruction /* inst */)
{
    m_halt = true;
    m_running = false;

    m_outfile << std::flush;
    dumpLog();

    return makePrePCState(m_pc);
}
