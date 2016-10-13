#pragma once

#include <iostream>
#include <fstream>
#include <array>
#include <unordered_map>
#include <functional>
#include <exception>
#include "opcode.hpp"

class Simulator
{
public:
    explicit Simulator(const std::string& binfile);
    void run();

private:
    std::ifstream m_binfile;
    int m_pc = 0;
    int64_t m_dynamic_inst_cnt = 0;

    /*
     * General purpose registers
     * R0 is zero register
     */
    static constexpr int REG_SIZE = 32;
    std::array<int32_t, REG_SIZE> m_reg = {{0}};

    // Floating point registers
    static constexpr int FREG_SIZE = 32;
    std::array<float, FREG_SIZE> m_freg = {{0}};

    // TODO Memory

    void initInst();

    // 32bit Instruction code
    using Instruction = uint32_t;

    Instruction fetch();
    OpCode decodeOpCode(Instruction);
    void exec(OpCode, Instruction);

    // Function for each instruction
    void nop(Instruction);
    void add(Instruction);
    void addi(Instruction);
    void sub(Instruction);

    std::unordered_map<OpCode,
        std::function<void(Instruction)>, OpCodeHash> m_inst_funcs;
    std::unordered_map<OpCode,
        int64_t, OpCodeHash> m_inst_cnt;

    // Operand
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

    OperandR decodeR(Instruction);
    OperandI decodeI(Instruction);

    /*
     * Extract bit string.
     * bitset(10110111000..., 0, 8) = 0..010110111
     */
    uint32_t bitset(uint32_t inst, int begin, int end);

    void printBitset(uint32_t bits, int begin, int end, bool endl = false);
    void printOperandR(const OperandR& op);
    void printOperandI(const OperandI& op);

    void printRegister();
};
