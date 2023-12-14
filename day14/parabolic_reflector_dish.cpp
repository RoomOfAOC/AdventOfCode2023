//
// Created by Harold on 2023/12/14.
//

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "../utils/hash.h"

using line_t = std::string;
using input_t = std::vector<line_t>;

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << '\n';
    return os;
}

input_t parse_input(char const* fn)
{
    std::ifstream ifs(fn);
    line_t line;
    input_t in{};
    while (std::getline(ifs, line))
        in.emplace_back(line);
    return in;
}

void roll(input_t& in)
{
    auto rows = in.size();
    auto cols = in[0].size();
    for (auto c = 0; c < cols; c++)
        for (auto _ = 0; _ < rows; _++)
            for (auto r = 1; r < rows; r++)
                if (in[r][c] == 'O' && in[r - 1][c] == '.') std::swap(in[r][c], in[r - 1][c]);
}

int load(input_t const& in)
{
    int sum = 0;
    int rows = in.size();
    for (auto r = 0; r < rows; r++)
        for (auto c : in[r])
            if (c == 'O') sum += rows - r;
    return sum;
}

int part_1(char const* fn)
{
    auto input = parse_input(fn);
    //std::cout << input << '\n';

    roll(input);
    //std::cout << input << '\n';

    return load(input);
}

input_t rotate(input_t const& in)
{
    auto rows = in.size();
    auto cols = in[0].size();
    input_t out{cols, line_t(rows, '.')};
    for (auto r = 0; r < rows; r++)
        for (auto c = 0; c < cols; c++)
            out[c][rows - 1 - r] = in[r][c];
    return out;
}

void roll_n(input_t& in)
{
    int rows = in.size();
    int cols = in[0].size();
    for (auto c = 0; c < cols; c++)
        for (auto r = 1; r < rows; r++)
        {
            if (in[r][c] == 'O' && in[r - 1][c] == '.')
            {
                // find most prev pos
                int prev = r - 1;
                while (prev > 0 && in[prev - 1][c] == '.')
                    prev--;
                std::swap(in[r][c], in[prev][c]);
            }
        }
}

void roll_s(input_t& in)
{
    int rows = in.size();
    int cols = in[0].size();
    for (auto c = 0; c < cols; c++)
        for (auto r = rows - 2; r >= 0; r--)
        {
            if (in[r][c] == 'O' && in[r + 1][c] == '.')
            {
                int prev = r + 1;
                while (prev < rows - 1 && in[prev + 1][c] == '.')
                    prev++;
                std::swap(in[r][c], in[prev][c]);
            }
        }
}

void roll_w(input_t& in)
{
    int rows = in.size();
    int cols = in[0].size();
    for (auto r = 0; r < rows; r++)
        for (auto c = 1; c < cols; c++)
        {
            if (in[r][c] == 'O' && in[r][c - 1] == '.')
            {
                int prev = c - 1;
                while (prev > 0 && in[r][prev - 1] == '.')
                    prev--;
                std::swap(in[r][c], in[r][prev]);
            }
        }
}

void roll_e(input_t& in)
{
    int rows = in.size();
    int cols = in[0].size();
    for (auto r = 0; r < rows; r++)
        for (auto c = cols - 2; c >= 0; c--)
        {
            if (in[r][c] == 'O' && in[r][c + 1] == '.')
            {
                int prev = c + 1;
                while (prev < cols - 1 && in[r][prev + 1] == '.')
                    prev++;
                std::swap(in[r][c], in[r][prev]);
            }
        }
}

int part_2(char const* fn)
{
    constexpr size_t cycles = 1'000'000'000;

    auto input = parse_input(fn);

    size_t c = 0;
    std::unordered_map<input_t, size_t, container_hash<input_t>> visited;
    while (c < cycles)
    {
        // rotate is slow
        // for (auto i = 0; i < 4; i++)
        // {
        //     roll(input);
        //     input = rotate(input);
        // }

        roll_n(input);
        roll_w(input);
        roll_s(input);
        roll_e(input);

        c++;

        if (visited.find(input) == visited.end())
            visited[input] = c;
        else
        {
            auto period = c - visited[input];
            c += (cycles - c) / period * period;
        }
    }
    //std::cout << input << '\n';

    return load(input);
}

int main(int argc, char* argv[])
{
    std::cout << "what is the total load on the north support beams?\n" << part_1(argv[1]) << std::endl;
    std::cout << "what is the total load on the north support beams?\n" << part_2(argv[1]) << std::endl;

    return 0;
}