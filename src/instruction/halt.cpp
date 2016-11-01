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
            ofstream ofs{"call_cnt.log"};
            ofs << m_dynamic_inst_cnt << endl;
            for (size_t i = 0; i < m_pc_called_cnt.size(); i++)
                ofs << 4 * i << ' ' << m_pc_called_cnt.at(i) << endl;
        }

        {
            ofstream ofs{"instruction.log"};
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

        if (m_output_memory) {
            ofstream ofs{"memory.log"};
            ofs << hex;
            for (auto m : m_memory)
                ofs << m << endl;
        }

        addstr("done!\n");
        refresh();
        // getch();
    }

    return *m_state_iter;
}
