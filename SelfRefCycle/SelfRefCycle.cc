#include <memory>
#include <iostream>

class MyClassA
{
public:
    MyClassA(int i, const std::shared_ptr<MyClassA> &nested = nullptr):
        i_(i),
        nested_(nested)
    {
        std::cout << "+MyClassA(" << i_ << ")" << std::endl;
    }
    ~MyClassA()
    {
        std::cout << "-MyClassA(" << i_ << ")" << std::endl;
    }

    std::shared_ptr<MyClassA> nested_;
    int i_;
};

int main()
{
    std::shared_ptr<MyClassA> ap(new MyClassA(100));
    std::shared_ptr<MyClassA> app = ap;

    MyClassA a(1, app);

    ap.reset();
    app.reset();

    std::shared_ptr<MyClassA> aaa(new MyClassA(999));
    aaa->nested_ = aaa; // No auto destruction. Why?

    return 0;
}

