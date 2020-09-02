// A Turing Machine Using C++ Template Meta Programming
//
// WARNING: Use clang++ (tested ver.7.0.1) or g++ (tested ver.8.3.0) will very
// like OOM on your machine.
//
// Reference:
// [1] Veldhuizen, Todd L. "C++ Templates are Turing Complete"
// [2] https://en.wikipedia.org/wiki/Busy_beaver
//
// Author: Hengyang Zhao <hzhao877502@gmail.com>
//
#include <iostream>

enum Symbol { _, X, O, };
enum State { A, B, C, D, Halt, };
enum Action { L, R, };

using Int = int;

std::ostream &operator<<(std::ostream &o, Symbol s) {
    switch (s) {
    case Symbol::X: return o << 'X';
    case Symbol::O: return o << 'O';
    case Symbol::_: return o << '.';
    default: throw 0;
    }
}

std::ostream &operator<<(std::ostream &o, State s) {
    switch (s) {
    case State::A: return o << 'A';
    case State::B: return o << 'B';
    case State::C: return o << 'C';
    case State::D: return o << 'D';
    case State::Halt: return o << "(Halted)";
    default: throw 0;
    }
}

std::ostream &operator<<(std::ostream &o, Action a) {
    switch (a) {
    case Action::L: return o << 'L';
    case Action::R: return o << 'R';
    default: throw 0;
    }
}

template <bool condition, typename TrueT, typename FalseT>
struct Cond { using Type = TrueT; };

template <typename TrueT, typename FalseT>
struct Cond<false, TrueT, FalseT> { using Type = FalseT; };

template <typename T>
struct Fmt {
    friend std::ostream &operator<<(std::ostream &o, const Fmt&) {
        T::Print(o);
        return o;
    }
};

template <Symbol current_symbol,
          State current_state,
          Symbol write_symbol,
          State next_state,
          Action action>
struct Rule {
    static constexpr Symbol CURRENT_SYMBOL  = current_symbol;
    static constexpr State  CURRENT_STATE   = current_state;
    static constexpr Symbol WRITE_SYMBOL    = write_symbol;
    static constexpr State  NEXT_STATE      = next_state;
    static constexpr Action ACTION          = action;

    template <Symbol symbol, State state>
    struct Matches {
        static constexpr bool VALUE = symbol == current_symbol
                                  and state == current_state;
    };
};

template <typename... Rules>
struct TransFunc {
    template <Symbol current_symbol, State current_state>
    struct Defined {
        static constexpr bool VALUE = false;
    };

    template <Symbol symbol, State state>
    struct GetAction {};

    template <Symbol symbol, State state>
    struct GetNextState {};

    template <Symbol symbol, State state>
    struct GetWriteSymbol {};
};

template <typename head_rule, typename... tail_rules>
struct TransFunc<head_rule, tail_rules...> {
    using TailFunc = TransFunc<tail_rules...>;

    template <Symbol symbol, State state>
    struct Defined {
        static constexpr bool VALUE =
            head_rule::template Matches<symbol, state>::VALUE ?
            true : TailFunc::template Defined<symbol, state>::VALUE;
    };

    template <Symbol symbol, State state>
    struct GetAction {
        static constexpr Action ACTION = Cond<
            head_rule::template Matches<symbol, state>::VALUE,
            head_rule,
            typename TailFunc::template GetAction<symbol, state>
        >::Type::ACTION;
    };

    template <Symbol symbol, State state>
    struct GetNextState {
        static constexpr State NEXT_STATE = Cond<
            head_rule::template Matches<symbol, state>::VALUE,
            head_rule,
            typename TailFunc::template GetNextState<symbol, state>
        >::Type::NEXT_STATE;
    };

    template <Symbol symbol, State state>
    struct GetWriteSymbol {
        static constexpr Symbol WRITE_SYMBOL = Cond<
            head_rule::template Matches<symbol, state>::VALUE,
            head_rule,
            typename TailFunc::template GetWriteSymbol<symbol, state>
        >::Type::WRITE_SYMBOL;
    };
};

template <Symbol... symbols>
struct Tape {
    static void Print(std::ostream &o) {};
    static constexpr Int SIZE = 0;

    template <Int head>
    struct Index {
        static constexpr Symbol SYMBOL = Symbol::_;
    };
};

template <Symbol head_symbol, Symbol... tail_symbols>
struct Tape<head_symbol, tail_symbols...> {
    using TailTape = Tape<tail_symbols...>;
    static void Print(std::ostream &o) { TailTape::Print(o << head_symbol); }
    static constexpr Int SIZE = TailTape::SIZE + 1;

    template <Int head>
    struct IndexNonZero {
        static constexpr Symbol SYMBOL =
            TailTape::template Index<head - 1>::SYMBOL;
    };

    struct IndexZero {
        static constexpr Symbol SYMBOL = head_symbol;
    };

    template <Int head>
    struct Index {
        static constexpr Symbol SYMBOL =
            Cond<head == 0, IndexZero, IndexNonZero<head>>::Type::SYMBOL;
    };
};

template <Symbol symbol, Int size>
struct PlainTape {
    static void Print(std::ostream &o) {
        for (int i = 0; i < size; ++i) o << symbol;
    };
    static constexpr Int SIZE = size;

    template <Int head>
    struct Index {
        static constexpr Symbol SYMBOL = symbol;
    };
};

template <typename... Tapes>
struct Concat { using Tape = ::Tape<>; };

template <typename HeadTape, typename... TailTapes>
struct Concat<HeadTape, TailTapes...> {
    using Tail = Concat<TailTapes...>;
    struct Tape {
        static void Print(std::ostream &o) {
            HeadTape::Print(o);
            Tail::Tape::Print(o);
        }
        static constexpr Int SIZE = HeadTape::SIZE + Tail::Tape::SIZE;

        template <Int head>
        struct Index {
            static constexpr Symbol SYMBOL = Cond<
                (head < HeadTape::SIZE),
                HeadTape,
                typename Tail::Tape
            >::Type::template Index<(head < HeadTape::SIZE)
                ? head
                : head - HeadTape::SIZE>::SYMBOL;
        };
    };
};

template <typename BaseTape, Int begin, Int end>
struct Slice {
    struct Trivial {
        using Tape = ::Tape<>;
    };

    struct NonTrivial {
        struct Tape {
            static constexpr Int SIZE = end - begin;

            struct EmptyHead {
                static void Print(std::ostream &o) {};
            };
            struct NonEmptyHead {
                static void Print(std::ostream &o) { o << Index<0>::SYMBOL; }
            };

            static void Print(std::ostream &o) {
                Cond<SIZE == 0, EmptyHead, NonEmptyHead>::Type::Print(o);
                Cond<(SIZE > 0),
                     typename Slice<Tape, 1, SIZE>::Tape,
                     ::Tape<>>::Type::Print(o);
            }

            template <Int head>
            struct Index {
                static constexpr Symbol SYMBOL =
                    BaseTape::template Index<head + begin>::SYMBOL;
            };
        };
    };

    using Tape = typename Cond<
        (0 <= begin and begin <= end and end <= BaseTape::SIZE),
        NonTrivial, Trivial
    >::Type::Tape;
};

template <typename BaseTape, Int head, Symbol symbol>
struct Write {
    struct ShouldExtend {
        using Tape = typename Concat<BaseTape, ::Tape<symbol>>::Tape;
    };

    struct ShouldUpdate {
        using Tape = typename Concat<
            typename Slice<BaseTape, 0, head>::Tape,
            ::Tape<symbol>,
            typename Slice<BaseTape, head + 1, BaseTape::SIZE>::Tape>::Tape;
    };

    using Tape = typename Cond<head == BaseTape::SIZE,
                               ShouldExtend, ShouldUpdate>::Type::Tape;
};

template <Int current, Action action>
struct MoveHead {};

template <Int current>
struct MoveHead<current, Action::L> {
    static constexpr Int NEW_HEAD = current - 1 < 0 ? 0 : current - 1;
};

template <Int current>
struct MoveHead<current, Action::R> {
    static constexpr Int NEW_HEAD = current + 1;
};

template <typename TapeIn, typename TransFuncIn, State state, Int head>
struct TuringMachine {
    using Tape = TapeIn;
    using TransFunc = TransFuncIn;
    static constexpr State STATE = state;
    static constexpr Int HEAD = head;

    static constexpr bool WILL_HALT = not TransFunc::template Defined<
        Tape::template Index<head>::SYMBOL, state
    >::VALUE;

    struct Next {
        static constexpr State NEXT_STATE = TransFunc::template GetNextState<
            Tape::template Index<head>::SYMBOL, state>::NEXT_STATE;

        static constexpr Int NEW_HEAD = MoveHead<
            head,
            TransFunc::template GetAction<Tape::template Index<head>::SYMBOL,
                                          state>::ACTION
        >::NEW_HEAD;

        using NewTape = typename Write<
            Tape,
            head,
            TransFunc::template GetWriteSymbol<
                Tape::template Index<head>::SYMBOL,
                state>::WRITE_SYMBOL
        >::Tape;

        using TuringMachine = ::TuringMachine<NewTape, TransFunc,
                                              NEXT_STATE, NEW_HEAD>;
    };

    struct Self {
        using TuringMachine = ::TuringMachine<Tape, TransFunc, STATE, HEAD>;
    };

    static constexpr Int TTL = WILL_HALT
        ? 0
        : Cond<WILL_HALT, Self, Next>::Type::TuringMachine::TTL + 1;

    static void Print(std::ostream &o) {
        o << "<Tape=" << Fmt<Tape>()
          << ", Head=" << HEAD
          << ", State=" << STATE
          << ", TTL=" << TTL
          << ">";
    }
};

template <typename TuringMachine, bool will_halt>
struct RunImpl {};

template <typename TuringMachine>
struct RunImpl<TuringMachine, true> {
    using Final = TuringMachine;
};

template <typename TuringMachine>
struct RunImpl<TuringMachine, false> {
    using Final = typename RunImpl<
        typename TuringMachine::Next::TuringMachine,
        TuringMachine::Next::TuringMachine::WILL_HALT
    >::Final;
};

template <typename TuringMachine>
struct Run {
    using Final = typename RunImpl<
        TuringMachine, TuringMachine::WILL_HALT>::Final;
};

int main()
{
    using BusyBeaver = TuringMachine<
        Concat<Tape<Symbol::O>,
               PlainTape<Symbol::_, 18>,
               Tape<Symbol::O>>::Tape,
        TransFunc<Rule<Symbol::_, State::A, Symbol::X, State::B,    Action::R>,
                  Rule<Symbol::X, State::A, Symbol::X, State::B,    Action::L>,
                  Rule<Symbol::_, State::B, Symbol::X, State::A,    Action::L>,
                  Rule<Symbol::X, State::B, Symbol::_, State::C,    Action::L>,
                  Rule<Symbol::_, State::C, Symbol::X, State::Halt, Action::R>,
                  Rule<Symbol::X, State::C, Symbol::X, State::D,    Action::L>,
                  Rule<Symbol::_, State::D, Symbol::X, State::D,    Action::R>,
                  Rule<Symbol::X, State::D, Symbol::_, State::A,    Action::R>>,
        State::A,
        13>;

    std::cout << Fmt<BusyBeaver>() << std::endl;
    std::cout << Fmt<Run<BusyBeaver>::Final>() << std::endl;
    return 0;
}
