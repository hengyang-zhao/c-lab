#include <iostream>

template <int N>
struct Fibonacci
{
    static constexpr long value = Fibonacci<N-1>::value + Fibonacci<N-2>::value;
};

template <>
struct Fibonacci<0> { static constexpr long value = 0L; };

template <>
struct Fibonacci<1> { static constexpr long value = 1L; };

#define PRINT_FIB(N) do {                                                    \
    std::cout << "Fibonacci[" #N "] = " << Fibonacci<N>::value << std::endl; \
} while (0)

int main()
{
    PRINT_FIB(0);
    PRINT_FIB(20);
    PRINT_FIB(40);
    PRINT_FIB(60);
    PRINT_FIB(80);
    return 0;
}
