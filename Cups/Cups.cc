#include <algorithm>
#include <iostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <vector>

template <class T>
T As(const std::string &text)
{
    std::stringstream ss(text);
    T ret;
    if (not(ss >> ret) or not ss.eof())
        throw std::invalid_argument(text);
    return ret;
}

template <class T>
std::string Str(const T &input)
{
    std::stringstream ss;
    ss << input;
    return ss.str();
}

void PourWater(int argc, char **argv)
{
    std::vector<unsigned> cup_sizes = { 0 /* unused */ };
    for (int i = 1; i < argc; ++i) {
        cup_sizes.push_back(As<unsigned>(argv[i]));
    }
    std::vector<unsigned> cup_levels(cup_sizes.size());

    std::string line;
    std::regex empty(R"(^\s*$)");
    std::regex pouring(R"(^\s*(\d*)\s*->\s*(\d*)\s*$)");
    std::smatch from_to;

    unsigned lineno = 0;
    while (std::getline(std::cin, line)) {

        lineno += 1;
        if (std::regex_match(line, empty)) continue;
        if (not std::regex_match(line, from_to, pouring))
            throw std::invalid_argument("Line " + Str(lineno) +
                                        ": invalid input '" + line + "'");

        std::string from = from_to[1];
        std::string to = from_to[2];

        if (from.empty() and to.empty()) {
            throw std::invalid_argument("Line " + Str(lineno) +
                                        ": should have at least one operand");
        } else if (from.empty()) {
            unsigned id = As<unsigned>(to);
            if (id < 1 or id > cup_sizes.size())
                throw std::invalid_argument("Line " + Str(lineno) +
                                            ": invalid cup id " + Str(id));
            std::cout << cup_sizes[id] - cup_levels[id] << std::endl;
            cup_levels[id] = cup_sizes[id];
        } else if (to.empty()) {
            unsigned id = As<unsigned>(from);
            if (id < 1 or id > cup_sizes.size())
                throw std::invalid_argument("Line " + Str(lineno) +
                                            ": invalid cup id " + Str(id));
            std::cout << cup_levels[id] << std::endl;
            cup_levels[id] = 0;
        } else {
            unsigned from_id = As<unsigned>(from);
            if (from_id < 1 or from_id > cup_sizes.size())
                throw std::invalid_argument("Line " + Str(lineno) +
                                            ": invalid cup id " + Str(from_id));
            unsigned to_id = As<unsigned>(to);
            if (to_id < 1 or to_id > cup_sizes.size())
                throw std::invalid_argument("Line " + Str(lineno) +
                                            ": invalid cup id " + Str(to_id));
            unsigned water = std::min(cup_levels[from_id],
                    cup_sizes[to_id] - cup_levels[to_id]);
            std::cout << water << std::endl;
            cup_levels[from_id] -= water;
            cup_levels[to_id] += water;
        }
    }
}

int main(int argc, char **argv)
{
    try {
        PourWater(argc, argv);
    } catch (const std::exception &e) {
        std::cerr << "Program error." << std::endl
                  << e.what() << std::endl;
    }
    return 0;
}

