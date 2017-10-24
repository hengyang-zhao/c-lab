#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

template <class T>
T lexical_cast(const std::string &s)
{
    std::stringstream ss(s);
    T ret;
    ss >> ret;
    if (not ss or not ss.eof()) throw std::invalid_argument(s);
    return ret;
}

struct Gem {
    int value;
    int size;
    struct BySize {
        bool operator()(const Gem &a, const Gem &b) const
        {
            return a.size < b.size;
        }
    };
};

std::istream &operator>>(std::istream &is, Gem &gem)
{
    is >> gem.value;
    is.ignore(1);
    is >> gem.size;
    return is;
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cerr << "Usage: Knapsack <max_size> <v1/s1> <v2/s2> ..."
                  << std::endl;
        return 1;
    }

    int max_size = lexical_cast<int>(argv[1]);

    std::vector<Gem> gems;
    for (int i = 2; i < argc; ++i) {
        gems.push_back(lexical_cast<Gem>(argv[i]));
    }

    std::sort(gems.begin(), gems.end(), Gem::BySize());
    std::map<std::pair<int, int>, int> dp_map;

    for (int i = 0; i < gems.size(); ++i)
    for (int j = 1; j <= max_size; ++j)
    {
        Gem &g = gems[i];
        std::pair<int, int> key = {i, j};

        if (g.size > max_size) {
            dp_map[key] = dp_map[std::make_pair(i - 1, j)];
        } else {
            dp_map[key] = std::max(
                    dp_map[std::make_pair(i - 1, j - g.size)] + g.value,
                    dp_map[std::make_pair(i - 1, j)]);
        }
    }

    std::cout << "Max value: "
              << dp_map[std::make_pair(gems.size() - 1, max_size)]
              << std::endl;
}
