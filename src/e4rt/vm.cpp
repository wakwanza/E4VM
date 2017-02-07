// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//

#include <stdio.h>
#include "e4rt/vm.h"
//#include "e4platf/mem.h"
#include "e4rt/process.h"

namespace e4 {

IMPL_EXCEPTION(FeatureMissing)
IMPL_EXCEPTION(CodeLoader)
IMPL_EXCEPTION(Scheduler)
IMPL_EXCEPTION(CodeServer)
IMPL_EXCEPTION(Process)

Term VM::add_atom(const String &atom_name) {
    auto atom_name_s = atom_name.c_str();

    // Try find atom (already exists)
    auto exists = atoms_.find(atom_name_s);
    if (exists.is_value()) { // duplicate
        return exists;
    }
    E4ASSERT(fits_in<Word>(atoms_.size())); // 64bit machine with 32bit words
    Word atom_id = static_cast<Word>(atoms_.size());

    // Add atom to direct lookup and reverse lookup table
    auto a = Term::make_atom(atom_id);
    atoms_.insert(a, atom_name_s);
    return a;
}

Node *VM::dist_this_node() {
#if E4FEATURE_ERLDIST
    E4TODO("implement Node and this node variable")
#endif
    return this_node_;
}

Process* VM::spawn(Term parent_pid, const MFArgs& mfargs) {
    (void)parent_pid;

    auto pid = sched_.make_pid();

    using platf::SingleAlloc;
    auto proc = SingleAlloc::alloc_class<Process>(*this, pid);
    auto apply_res = proc->apply(mfargs);
    apply_res.assert();

    sched_.register_proc(proc);
    return proc;
}

#if E4DEBUG
void VM::print(Term t) const {
    switch (t.as_primary_.tag_) {
        case primary_tag::Immediate: {
            print_imm(t);
        } break;
        case primary_tag::Boxed: {} break;
        case primary_tag::Header: {} break;
        case primary_tag::Cons: {} break;
    }
}
#endif // DEBUG

#if E4DEBUG
void VM::print_imm(Term t) const {
    switch (t.as_imm_.imm_tag_) {
        case immediate_tag::Atom: {
            debug_printf("'%s'", find_atom(t));
        } break;
        case immediate_tag::SmallInt: {} break;
        case immediate_tag::ShortPid: {} break;
        case immediate_tag::ShortPort: {} break;
        case immediate_tag::FpRegister: {} break;
        case immediate_tag::Catch: {} break;
        case immediate_tag::XRegister: {} break;
        case immediate_tag::YRegister: {} break;
        case immediate_tag::Special: {} break;
    }
}

const char* VM::find_atom(Term atom) const {
    E4ASSERT(atom.is_atom());
    return atoms_.find(atom);
}
#endif // DEBUG

#define VMDBG(T) e4::debug_printf("vm loop: " T "\n")

void VM::run() {
    auto proc = sched_.next();
    if (not proc) {
        VMDBG("idle");
        return; // nothing to do
        // TODO: perform maintenance tasks, enter energy saving idle
    }

    auto& context_ = proc->context_;
    J1Opcode8 instr8 = context_.fetch();

    switch (instr8.unsigned_.instr_tag_) {

        // A common instruction
        case j1_instr_tag::JUMP: {
            // TODO: Remap jmp destinations on code load or implement relative jumps
            proc->jump(j1jump_addr(instr8, context_.fetch()));
        } break;

        // A common instruction
        case j1_instr_tag::JUMP_COND: {
            if (context_.ds_.pop() == 0) {
                proc->jump(j1jump_addr(instr8, context_.fetch()));
            }
        } break;

        // A common instruction
        case j1_instr_tag::LITERAL_SMALL_POS_INTEGER: {
            // TODO: implement literal values longer than 15 bits signed
            auto val = instr8.unsigned_.val_;
            context_.ds_.push((Word)val);
        } break;

        // A rare instruction (SWAPs, stray RETs etc)
        case j1_instr_tag::ALU: {
            auto instr16 = context_.fetch(instr8);
            run_alu(instr16);
        } break;

        // A rare instruction (Forth call)
        case j1_instr_tag::CALL: {
            context_.rs_.push(context_.pc_.as_word());
            proc->jump(j1jump_addr(instr8, context_.fetch()));
        } break;
    }
}

inline void VM::run_alu(J1Opcode16 instr) {

}

//static void print_atoms_helper(const void* k, const void* v, void* extra) {
//    auto pk = static_cast<const Term*>(k);
//    auto pv = static_cast<const CString*>(v);
//    auto vm = static_cast<VM*>(extra);
//    vm->print(*pk);
//    ::printf(" => %s\n", pv->str());
//}
//
//void VM::print_atoms() const {
//    atoms_.visit_nodes(print_atoms_helper, const_cast<VM*>(this));
//}


} // ns e4
