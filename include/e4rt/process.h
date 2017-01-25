//
// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
//
#pragma once

#include "e4rt/vm.h"
#include "e4rt/term.h"
#include "e4rt/bytecode.h"
#include "e4std/complicated.h"

namespace e4 {
using e4std::VoidResult;

// Stack implementation
// TODO: stack base pointer to address args and variables together
// TODO: some generic stack frame implementation for enter/leave. Dynamic heap frames?
class Stack {
private:
    PODVector<Word> cells_;
public:
    Stack() = default;
    // Will grow using vector realloc
    void push_term(Term t) {
        push(t.get_raw());
    }

    void push(Word w) {
        cells_.push_back(w);
    }

    // Will shrink size but not shrink memory
    Word pop() {
        E4ASSERT(not cells_.empty());
        auto val = cells_.back();
        cells_.resize(cells_.size() - 1);
        return val;
    }

    Term pop_term() {
        return Term(pop());
    }
};

constexpr Word INIT_PROCESS_HEAP = 64; // first size for process heap (words)

// VM runtime context which gets swapped into VM loop and out
class RuntimeContext {
public:
    CodeAddress pc_;
    Stack ds_;      // data stack
    Stack rs_;      // return stack
    RuntimeContext() = default;

    J1Opcode fetch() {
        auto opcode = pc_.fetch();
        // TODO: check code end/code range?
        pc_.advance();
        return opcode;
    }
};

class Process {
private:
    Term pid_;
    Heap heap_;
    VM& vm_;
    // [pid()] -- linked processes
    Term links_ = NIL;
    // [pid()] -- processes which monitor this process
    Term monitors_ = NIL;

public:
    RuntimeContext context_;

public:
    Process() = delete;
    explicit Process(VM& vm, Term pid)
            : pid_(pid), heap_(INIT_PROCESS_HEAP), vm_(vm), context_() {}

    Term self() const { return pid_; }

    // Sets arguments and enters mfarity with args, does not wait for execution
    // but just sets instruction pointer instead
    E4_NODISCARD VoidResult
    apply(const MFArgs& mfargs);

    // TODO: maybe belongs to runtime context
    void jump(CodeAddress newpc) {
        E4LOG1("[proc] jump 0x%zx\n", newpc.ptr_);
        context_.pc_ = newpc;
    }

    // TODO: maybe belongs to runtime context
    void jump_offset(SignedWord offs) {
        E4LOG1("[proc] jump-rel 0x%zd\n", offs);
        context_.pc_ += offs;
    }
};

} // ns e4