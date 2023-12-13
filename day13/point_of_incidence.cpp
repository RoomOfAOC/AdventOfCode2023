//
// Created by Harold on 2023/12/13.
//

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>

using pattern_t = std::vector<std::string>;
using input_t = std::vector<pattern_t>;

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << '\n';
    return os;
}

input_t parse_input(char const* fn, int unfold = 1)
{
    input_t input{};
    std::ifstream ifs(fn);
    std::string line;
    pattern_t p{};
    while (std::getline(ifs, line))
        if (line.empty())
        {
            input.push_back(p);
            p.clear();
        }
        else
            p.emplace_back(line);
    input.emplace_back(p);
    return input;
}

pattern_t transpose(pattern_t const& p)
{
    pattern_t rp{};
    std::string l{};
    l.reserve(p[0].size());
    for (auto i = 0; i < p[0].size(); i++)
    {
        l.clear();
        for (auto const& r : p)
            l += r[i];
        rp.push_back(l);
    }
    return rp;
}

template <bool with_smudge = false> unsigned horizontal_reflection(pattern_t const& p)
{
    for (size_t i = 0; i < p.size() - 1; i++)
    {
        auto length = std::min(i + 1, p.size() - i - 1);
        if constexpr (with_smudge)
        {
            size_t cnt = 0;
            auto it_d = p.begin() + i + 1;
            auto it_u = p.rend() - 1 - i;
            for (; it_d != p.begin() + i + 1 + length; it_d++, it_u++)
                for (auto j = 0; j < it_d->size(); j++)
                    if ((*it_d)[j] == (*it_u)[j]) cnt++;
            // extractly one smudge
            if (cnt + 1 == p[0].size() * length) return i + 1;
        }
        else
        {
            if (std::equal(p.begin() + i + 1, p.begin() + i + 1 + length, p.rend() - 1 - i)) return i + 1;
        }
    }
    return 0;
}

template <bool with_smudge = false> unsigned vertical_reflection(pattern_t const& p)
{
    return horizontal_reflection<with_smudge>(transpose(p));
}

size_t part_1(char const* fn)
{
    auto input = parse_input(fn);
    //std::cout << input << '\n';

    size_t sum = 0;
    for (auto const& p : input)
        sum += 100 * horizontal_reflection(p) + vertical_reflection(p);

    return sum;
}

size_t part_2(char const* fn)
{
    auto input = parse_input(fn);

    size_t sum = 0;
    for (auto const& p : input)
        sum += 100 * horizontal_reflection<true>(p) + vertical_reflection<true>(p);

    return sum;
}

int main(int argc, char* argv[])
{
    std::cout << "What number do you get after summarizing all of your notes?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What number do you get after summarizing the new reflection line in each pattern in your notes?\n"
              << part_2(argv[1]) << std::endl;

    return 0;
}