#include <iostream>
#include <vector>

int main()
{
    std::vector<uint64_t> known_primes;

    for (uint64_t n(2), i(1);; ++n) {
        bool is_prime = true;
        for (uint64_t p: known_primes) {
            if (p * p > n) break;
            if (n % p == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            known_primes.push_back(n);
            if (i % 2000 == 0) {
                std::cout << "Prime[" << i/1000 << "K] = " << n << "\r" << std::flush;
            }
            i += 1;
        }
    }

    return 0;
}
