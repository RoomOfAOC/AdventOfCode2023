//
// Created by Harold on 2023/12/11.
//

#include "../utils/mat.h"
#include <algorithm>
#include <utility>

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << '\n';
    return os;
}

using expanded_rc_t = std::pair<std::vector<size_t>, std::vector<size_t>>;
using galaxies_t = std::vector<Point>;

std::pair<expanded_rc_t, galaxies_t> parse_input(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    std::vector<std::string> input;
    expanded_rc_t expanded_rc{};
    while (std::getline(ifs, line))
    {
        input.push_back(line);
        // expand row
        if (line.find('#') == std::string::npos) expanded_rc.first.push_back(input.size() - 1);
    }
    for (auto i = 0; i < input[0].size(); i++)
    {
        std::string col;
        for (auto j = 0; j < input.size(); j++)
            col += input[j][i];
        // expand col
        if (col.find('#') == std::string::npos) expanded_rc.second.push_back(i);
    }
    // std::cout << input << '\n';

    auto rows = input.size(), cols = input[0].size();
    std::vector<Point> galaxies;
    for (auto i = 0; i < rows; i++)
        for (auto j = 0; j < cols; j++)
            if (input[i][j] == '#') galaxies.push_back({i, j});

    // std::cout << galaxies;

    return std::make_pair(expanded_rc, galaxies);
}

size_t get_sum(char const* fn, size_t dist)
{
    auto [expanded_rc, galaxies] = parse_input(fn);
    // std::cout << expanded_rc.first << '\n';
    // std::cout << expanded_rc.second << '\n';
    size_t sum = 0;
    for (auto i = 0; i < galaxies.size() - 1; i++)
        for (auto j = i + 1; j < galaxies.size(); j++)
        {
            auto x1 = galaxies[i].x, x2 = galaxies[j].x, y1 = galaxies[i].y, y2 = galaxies[j].y;
            if (x1 > x2) std::swap(x1, x2);
            if (y1 > y2) std::swap(y1, y2);
            sum += x2 - x1 + y2 - y1;
            for (auto x = x1; x < x2; x++)
                if (std::binary_search(expanded_rc.first.cbegin(), expanded_rc.first.cend(), x)) sum += dist;
            for (auto y = y1; y < y2; y++)
                if (std::binary_search(expanded_rc.second.cbegin(), expanded_rc.second.cend(), y)) sum += dist;
        }
    return sum;
}

size_t part_1(char const* fn)
{
    return get_sum(fn, 1);
}

size_t part_2(char const* fn)
{
    return get_sum(fn, 1'000'000 - 1);
}

int main(int argc, char* argv[])
{
    std::cout << "What is the sum of these lengths?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What is the sum of these lengths?\n" << part_2(argv[1]) << std::endl;

    return 0;
}