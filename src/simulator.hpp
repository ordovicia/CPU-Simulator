#pragma once

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <unordered_set>
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
        bool output_memory);

    void run();

private:
    const std::string m_binfile_name;
    std::ifstream m_binfile;
    const bool m_interactive;
    const bool m_output_memory;

    bool m_halt = false;
    std::unordered_set<int64_t> m_breakpoints;

    std::ifstream m_infile;
    std::ofstream m_outfile;

    // Instruction
    using Instruction = uint32_t;  // 32bit Instruction code
    std::vector<Instruction> m_codes;

    int64_t m_dynamic_inst_cnt = 0;
    std::vector<int64_t> m_pc_called_cnt;

    // State
    static constexpr size_t MEMORY_SIZE = 1000000;
    std::array<int32_t, MEMORY_SIZE> m_memory = {{}};

    struct MemoryPatch {
        MemoryPatch() = default;
        MemoryPatch(bool v, int32_t i, int32_t p)
            : valid(v), idx(i), pre_val(p) {}

        bool valid = false;
        int32_t idx;
        int32_t pre_val;
    };

    static constexpr int REG_SIZE = 32;
    static constexpr int FREG_SIZE = 32;

    struct State {
        int64_t pc = 0;

        /*
         * General purpose registers
         * R0 is zero register
         */
        std::array<int32_t, REG_SIZE> reg = {{}};

        // Floating point registers
        std::array<float, FREG_SIZE> freg = {{}};

        MemoryPatch memory_patch = {};
    };

    // State history
    static constexpr size_t STATE_HIST_SIZE = 256;
    SizedDeque<State, STATE_HIST_SIZE> m_state_hist;
    using StateIter = typename decltype(m_state_hist)::Iterator;
    StateIter m_state_iter;

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
     * | opcode | rs | rt | rd | shamt |(fanct)|
     * -----------------------------------------
     * | 6      | 5  | 5  | 5  | 5     |(6    )|
     * -----------------------------------------
     */
    struct OperandR {
        uint32_t rs;
        uint32_t rt;
        uint32_t rd;
        uint32_t shamt;
        uint32_t fanct;
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

    // Function for each instruction
    std::unordered_map<OpCode, std::function<State(Instruction)>> m_inst_funcs;
    // Instruction called counter
    std::unordered_map<OpCode, int64_t> m_inst_cnt;
    void initInstruction();

    static OpCode decodeOpCode(Instruction);
    State exec(OpCode, Instruction);

    static OperandR decodeR(Instruction);
    static OperandI decodeI(Instruction);
    static OperandJ decodeJ(Instruction);

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

    static void printHelp();

#include "instruction.hpp"
};
