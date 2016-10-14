#include "simulator.hpp"

Simulator::State Simulator::nop(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;

    OperandR op = decodeR(inst);
    printOperandR(op);

    auto new_state = now_state;
    new_state.pc++;
    return new_state;
}

Simulator::State Simulator::add(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;

    OperandR op = decodeR(inst);
    printOperandR(op);
    auto ans = now_state.reg.at(op.rs) + now_state.reg.at(op.rt);

    auto new_state = now_state;
    new_state.pc++;
    new_state.reg.at(op.rd) = ans;
    return new_state;
}

Simulator::State Simulator::addi(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;

    OperandI op = decodeI(inst);
    printOperandI(op);
    auto ans = now_state.reg.at(op.rs) + op.immediate;

    auto new_state = now_state;
    new_state.pc++;
    new_state.reg.at(op.rt) = ans;
    return new_state;
}

Simulator::State Simulator::sub(Instruction inst, StateIter state_iter)
{
    auto now_state = *state_iter;

    OperandR op = decodeR(inst);
    printOperandR(op);
    auto ans = now_state.reg.at(op.rs) - now_state.reg.at(op.rt);

    auto new_state = now_state;
    new_state.pc++;
    new_state.reg.at(op.rd) = ans;
    return new_state;
}

Simulator::State Simulator::halt(Instruction inst, StateIter state_iter)
{
    OperandR op = decodeR(inst);
    printOperandR(op);
    m_halt = true;

    return *state_iter;
}
