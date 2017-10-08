#include <iostream>

#include "Hook.h"

template<class Entity>
class HookA: public Hook<Entity, HookA<Entity>> {};

template<class Entity>
class HookB: public Hook<Entity, HookB<Entity>> {};

class DataBlockBase
{
    friend std::ostream &operator<<(std::ostream &os, const DataBlockBase& db) {
        os << db.data_;
        return os;
    }
protected:
    DataBlockBase(const std::string &data) { data_ = data; }
    std::string data_;
};

class HookedDataBlock: public DataBlockBase,
                       public Hook<HookedDataBlock>
{
public:
    HookedDataBlock(const std::string &data): DataBlockBase(data) {}
};

class DoubleHookedDataBlock: public DataBlockBase,
                             public HookA<DoubleHookedDataBlock>,
                             public HookB<DoubleHookedDataBlock>
{
public:
    DoubleHookedDataBlock(const std::string &data): DataBlockBase(data) {}
};

void TestSingleHook()
{
    HookedDataBlock a("A"), b("B"), c("C"), d("D"), e("E");
    a.HookToLeft(&b);
    b.HookToLeft(&c);
    c.HookToLeft(&d);
    d.HookToLeft(&e);
    e.HookToLeft(&a);

    HookedDataBlock *db = &a;
    std::cout << "Loop:" << std::endl;
    for (int i = 0; i < 7; ++i) {
        std::cout << *db << std::endl;
        db = db->Right();
    }
}

void TestDoubleHook()
{
    DoubleHookedDataBlock a("A");
    DoubleHookedDataBlock b("B");
    DoubleHookedDataBlock c("C");
    DoubleHookedDataBlock d("D");
    DoubleHookedDataBlock e("E");
    DoubleHookedDataBlock f("F");
    DoubleHookedDataBlock g("G");

    a.HookA::HookToLeft(&b);
    b.HookA::HookToLeft(&c);
    c.HookA::HookToLeft(&d);
    d.HookA::HookToLeft(&a);

    d.HookB::HookToLeft(&e);
    e.HookB::HookToLeft(&c);
    c.HookB::HookToLeft(&f);
    f.HookB::HookToLeft(&g);
    g.HookB::HookToLeft(&d);

    std::cout << "Loop A:" << std::endl;
    {
        DoubleHookedDataBlock *db = &d;
        for (int i = 0; i < 5; ++i) {
            std::cout << *db << std::endl;
            db = db->HookA::Right();
        }
    }

    std::cout << "Loop B:" << std::endl;
    {
        DoubleHookedDataBlock *db = &d;
        for (int i = 0; i < 6; ++i) {
            std::cout << *db << std::endl;
            db = db->HookB::Right();
        }
    }
}

int main(int argc, char **argv)
{
    std::cout << "Hello. This is a multi-inheritance example." << std::endl;

#define PRINT_SIZEOF(type) do {                                         \
    std::cout << "sizeof(" #type ") = " << sizeof(type) << std::endl;   \
} while(0)

    PRINT_SIZEOF(int);
    PRINT_SIZEOF(std::string);
    PRINT_SIZEOF(DataBlockBase);
    PRINT_SIZEOF(Hook<HookedDataBlock>);
    PRINT_SIZEOF(HookedDataBlock);
    PRINT_SIZEOF(HookA<DoubleHookedDataBlock>);
    PRINT_SIZEOF(HookA<DoubleHookedDataBlock>);
    PRINT_SIZEOF(DoubleHookedDataBlock);

    TestSingleHook();
    TestDoubleHook();
}

