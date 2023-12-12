//
// Created by Harold on 2023/12/12.
//

#include <fstream>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <unordered_map>

#include "../utils/str_op.h"
#include "../utils/hash.h"

using line_t = std::pair<std::string, std::vector<int>>;
using input_t = std::vector<line_t>;

std::ostream& operator<<(std::ostream& os, line_t const& l)
{
    os << l.first << " [ ";
    for (auto i : l.second)
        os << i << " ";
    os << "]";
    return os;
}

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
    while (std::getline(ifs, line) && !line.empty())
    {
        line_t l{};
        auto sv = split(std::string_view(line));
        for (auto i = 0; i < unfold; i++)
        {
            l.first += sv[0];
            l.first += '?';
        }
        l.first.pop_back();

        auto nums = split(sv[1], ",");
        for (auto i = 0; i < unfold; i++)
            for (auto ssv : nums)
                l.second.emplace_back(parse_num<int>(ssv));
        input.emplace_back(l);
    }
    return input;
}

struct state_t
{
    int sharp_num{};
    int spring_idx{};
    int group_idx{};

    bool operator==(state_t const& other) const
    {
        return sharp_num == other.sharp_num && spring_idx == other.spring_idx && group_idx == other.group_idx;
    }

    struct hasher
    {
        size_t operator()(state_t const& s) const
        {
            std::size_t h = 0;
            hash_combine(h, s.sharp_num, s.spring_idx, s.group_idx);
            return h;
        }
    };
};

std::ostream& operator<<(std::ostream& os, state_t const& s)
{
    return os << "{" << s.sharp_num << ", " << s.spring_idx << ", " << s.group_idx << "}";
}

size_t possible_arrangements(line_t const& l)
{
    std::unordered_map<state_t, size_t, state_t::hasher> visited{};
    state_t state{};
    auto const& [record, group] = l;
    int spring_num = record.size();
    int group_num = group.size();

    auto count = [&record, &group, &visited, spring_num, group_num](state_t state, auto&& count) -> size_t {
        if (visited.find(state) != visited.end()) return visited[state];

        //std::cout << state << '\n';

        auto [sn, s, g] = state;

        // reach record end
        if (s == spring_num)
        {
            if ((g == group_num - 1 && sn == group[g]) || (g == group_num && sn == 0)) return 1;
            return 0;
        }

        size_t cnt = 0;

        // ? or .
        if (record[s] != '#')
        {
            if (sn == 0) // no '#' found yet, still search in this group
                cnt += count({0, s + 1, g}, count);
            else if (sn == group[g]) // all '#' found, go next group
                cnt += count({0, s + 1, g + 1}, count);
        }

        // ? -> #
        if (record[s] != '.')
            if (g < group_num && sn < group[g]) // not all '#' found in this group, search next
                cnt += count({sn + 1, s + 1, g}, count);

        // all checked
        visited[state] = cnt;
        return cnt;
    };

    return count(state, count);
}

size_t part_1(char const* fn)
{
    auto input = parse_input(fn);
    //std::cout << input << '\n';

    return std::accumulate(input.cbegin(), input.cend(), size_t{},
                           [](size_t s, line_t const& l) { return s + possible_arrangements(l); });
}

size_t part_2(char const* fn)
{
    auto input = parse_input(fn, 5);
    //std::cout << input << '\n';

    return std::accumulate(input.cbegin(), input.cend(), size_t{},
                           [](size_t s, line_t const& l) { return s + possible_arrangements(l); });
}

int main(int argc, char* argv[])
{
    std::cout << "What is the sum of those counts?\n" << part_1(argv[1]) << std::endl;
    std::cout << "what is the new sum of possible arrangement counts?\n" << part_2(argv[1]) << std::endl;

    return 0;
}