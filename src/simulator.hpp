#pragma once

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <unordered_map>
#include <functional>
#include <string>
#include "sized_deque.hpp"
#include "opcode.hpp"

class Simulator
{
public:
    explicit Simulator(
        const std::string& binfile,
        const std::string& infile,
        bool interactive,
        bool output_memory,
        bool prev_enable);

    void run();
    void disasm();

private:
    const std::string m_binfile_name;
    std::ifstream m_binfile;
    const bool m_interactive;
    const bool m_output_memory;
    const bool m_prev_enable;

    std::ifstream m_infile;
    std::ofstream m_outfile;

    bool m_halt = false;
    bool m_running = false;

    // breakpointの、PCとdelay（N回通ったらbreak）のマップ
    std::unordered_map<int64_t, int64_t> m_breakpoints;
    void inputBreakpoint(char* input);

    // Instruction
    using Instruction = uint32_t;  // 32bit Instruction code
    std::vector<Instruction> m_codes;

    int64_t m_dynamic_inst_cnt = 0;
    std::vector<int64_t> m_pc_called_cnt;  // PCごとの実行回数

    // State
    int64_t m_pc = 0;

    /*
     * General purpose registers.
     * R0 is zero register
     */
    static constexpr int REG_NUM = 32;
    std::array<int32_t, REG_NUM> m_reg = {{}};

    // Floating point registers
    static constexpr int FREG_NUM = 32;
    std::array<float, FREG_NUM> m_freg = {{}};

    static constexpr size_t MEMORY_NUM = 1000000;
    std::array<int32_t, MEMORY_NUM> m_memory = {{}};

    // State history
    struct PreState {
        struct PCReg {
            bool changed = false;
            int64_t preval;
        } pc;

        struct GPReg {
            bool changed = false;
            size_t idx;
            int32_t preval;
        } gpreg;

        struct FReg {
            bool changed = false;
            size_t idx;
            float preval;
        } freg;

        struct Mem {
            bool changed = false;
            size_t idx;
            int32_t preval;
        } mem;
    };

    PreState makePrePCState(int64_t pc) const
    {
        PreState pre_state;
        pre_state.pc.changed = true;
        pre_state.pc.preval = pc;
        return pre_state;
    }

    PreState makePreGPRegState(size_t idx) const
    {
        PreState pre_state;

        pre_state.pc.changed = true;
        pre_state.pc.preval = m_pc;

        pre_state.gpreg.changed = true;
        pre_state.gpreg.idx = idx;
        pre_state.gpreg.preval = m_reg.at(idx);
        return pre_state;
    }

    PreState makePreFRegState(size_t idx) const
    {
        PreState pre_state;

        pre_state.pc.changed = true;
        pre_state.pc.preval = m_pc;

        pre_state.freg.changed = true;
        pre_state.freg.idx = idx;
        pre_state.freg.preval = m_freg.at(idx);
        return pre_state;
    }

    PreState makeMemPreState(size_t idx) const
    {
        PreState pre_state;

        pre_state.pc.changed = true;
        pre_state.pc.preval = m_pc;

        pre_state.mem.changed = true;
        pre_state.mem.idx = idx;
        pre_state.mem.preval = m_memory.at(idx);
        return pre_state;
    }

    static constexpr size_t STATE_HIST_NUM = 256;
    SizedDeque<PreState, STATE_HIST_NUM> m_state_hist;
    decltype(m_state_hist)::Iterator m_state_hist_iter;

    void reset();

    // Operand
    enum class OperandType {
        R,
        I,
        J,
        N,  // do not care
    };

    /*
     * Type R:
     * -----------------------------------------
     * | opcode | rs | rt | rd | shamt |       |
     * -----------------------------------------
     * | 6      | 5  | 5  | 5  | 5     |       |
     * -----------------------------------------
     */
    struct OperandR {
        uint32_t rs;
        uint32_t rt;
        uint32_t rd;
        uint32_t shamt;
    };

    /*
     * Type I:
     * -----------------------------------------
     * | opcode | rs | rt | immediate          |
     * -----------------------------------------
     * | 6      | 5  | 5  | 16                 |
     * -----------------------------------------
     */
    struct OperandI {
        uint32_t rs;
        uint32_t rt;
        uint32_t immediate;
    };

    /*
     * Type J:
     * -----------------------------------------
     * | opcode | addr                         |
     * -----------------------------------------
     * | 6      | 26                           |
     * -----------------------------------------
     */
    struct OperandJ {
        uint32_t rs;
        uint32_t addr;
    };

    // Instruction called counter
    std::unordered_map<OpCode, int64_t> m_inst_cnt;

    static OpCode decodeOpCode(Instruction);

    PreState exec(OpCode, Instruction);
    PreState execInst(OpCode, Instruction);

    static OperandR decodeR(Instruction);
    static OperandI decodeI(Instruction);
    static OperandJ decodeJ(Instruction);

    // disasm
    struct Mnemonic {
        std::string mnemonic;
        OperandType type;
        // clang-format off
        enum class OperandField { N, R, I, F, };  // clang-format on
        std::array<OperandField, 4> operand_field;
    };

    std::unordered_map<OpCode, Mnemonic> m_mnemonic_table;
    void initDisassembler();

    std::string disasm(Instruction) const;

    void dumpLog() const;


    // print
    static void printBitset(
        uint32_t bits, int begin = 0, int end = 32, bool endl = false);

    struct Screen {
        int width = 0, height = 0;
        int col_num = 0;
        int code_window_len = 0;

        void update();
        void printBoarder(char c, bool p = true) const;
    } m_screen;

    void printState() const;
    void printCode() const;
    void printBreakPoints() const;

    void printHelp() const;

#include "instructions.hpp"
};
