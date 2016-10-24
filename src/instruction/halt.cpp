#include <ncurses.h>
#include "simulator.hpp"

Simulator::State Simulator::halt(Instruction /* inst */)
{
    m_halt = true;

    if (m_run) {
        using namespace std;

        addstr("Outputting stat info... ");
        refresh();

        {
            ofstream ofs{"instruction.log"};
            ofs << m_dynamic_inst_cnt << endl;
            for (auto inst : m_inst_cnt)
                ofs << static_cast<uint32_t>(inst.first) << ' '
                    << inst.second << endl;
        }

        {
            ofstream ofs{"register.log"};
            ofs << hex;
            for (auto r : m_state_iter->reg)
                ofs << r << endl;
            for (auto r : m_state_iter->freg)
                ofs << r << endl;
        }

        {
            ofstream ofs{"memory.log"};
            ofs << hex;
            for (auto m : m_memory)
                ofs << m << endl;
        }

        addstr("done!\n");
        refresh();
        getch();
    }

    return *m_state_iter;
}
