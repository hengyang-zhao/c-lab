#include <iostream>
#include <limits>
#include <string>

enum class Endian { BIG, LITTLE };
std::ostream &operator<<(std::ostream &out, const Endian endian)
{
    switch (endian) {
    case Endian::BIG: return out << "Endian::BIG";
    case Endian::LITTLE: return out << "Endian::LITTLE";
    default: return out << "(Unknown Endian)";
    }
}

Endian GetEndianType()
{
    static const unsigned long data = 1UL;
    if (reinterpret_cast<const unsigned char *>(&data)[0] == '\1')
        return Endian::LITTLE;
    else
        return Endian::BIG;
}

template <class T>
std::string BinaryForm(const T &value)
{
    static const Endian endian = GetEndianType();
    static const int char_width = 8;
    std::string bin_form = "0b";

    const char *data = reinterpret_cast<const char *>(&value);

    for (int i = 0; i < sizeof(T); ++i) {
        bin_form.push_back('_');

        int index = (endian == Endian::BIG) ? i : (sizeof(T) - i - 1);
        for (int j = char_width - 1; j >= 0; --j)
            bin_form.push_back((data[index] & (0x1U << j)) ? '1' : '0');
    }

    return bin_form;
}

int main()
{
    std::cout << GetEndianType() << std::endl;
    std::cout << BinaryForm(1.0f) << std::endl;
    std::cout << BinaryForm(-1.0f) << std::endl;
    std::cout << BinaryForm(0.25f) << std::endl;
    std::cout << BinaryForm(-0.25f) << std::endl;
    std::cout << BinaryForm(0.1f) << std::endl;
    std::cout << BinaryForm(-0.1f) << std::endl;
    std::cout << BinaryForm(16777215.0f) << std::endl;
    std::cout << BinaryForm(-16777215.0f) << std::endl;

    std::cout << BinaryForm(16777216.0f) << std::endl;
    std::cout << BinaryForm(-16777216.0f) << std::endl;

    std::cout << BinaryForm(16777215.0) << std::endl; // DOUBLE PRECISION!
    std::cout << BinaryForm(-16777215.0) << std::endl; // DOUBLE PRECISION!

    std::cout << BinaryForm(0.0f) << std::endl;
    std::cout << BinaryForm(-0.0f) << std::endl;
    std::cout << BinaryForm(std::numeric_limits<float>::infinity()) << std::endl;
    std::cout << BinaryForm(-std::numeric_limits<float>::infinity()) << std::endl;
    std::cout << BinaryForm(std::numeric_limits<float>::quiet_NaN()) << std::endl;
    std::cout << BinaryForm(-std::numeric_limits<float>::quiet_NaN()) << std::endl;
}
