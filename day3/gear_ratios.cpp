//
// Created by Harold on 2023/12/3.
//

#include <fstream>
#include <string>
#include <string_view>
#include <charconv>
#include <vector>
#include <algorithm>
#include <iostream>

struct num_t
{
    int value;
    int row;
    std::vector<int> cols;
};

struct symbol_t
{
    char value;
    int row;
    int col;
};

using numbers_and_symbols_t = std::pair<std::vector<num_t>, std::vector<symbol_t>>;

std::ostream& operator<<(std::ostream& os, numbers_and_symbols_t const& ns)
{
    os << "numbers:\n";
    for (auto const& n : ns.first)
        os << "\t{" << n.value << ", " << n.row << ", " << n.cols[0] << " - " << n.cols[n.cols.size() - 1] << "}\n";
    os << "symbols:\n";
    for (auto const& s : ns.second)
        os << "\t(" << s.value << ", " << s.row << ", " << s.col << ")\n";
    return os;
}

numbers_and_symbols_t parse_input(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    num_t n;
    int r = 0;
    numbers_and_symbols_t res{};
    while (std::getline(ifs, line))
    {
        line += '.'; // for counting case like ...111
        n.value = 0;
        n.row = r;
        n.cols.clear();
        for (auto c = 0; c < line.size(); c++)
            if (std::isdigit(line[c]))
            {
                n.value = n.value * 10 + (line[c] - '0');
                n.cols.push_back(c);
            }
            else
            {
                if (n.value != 0)
                {
                    res.first.push_back(n);
                    n.value = 0;
                    n.cols.clear();
                }
                if (line[c] != '.') res.second.push_back({line[c], r, c});
            }
        r++;
    }
    return res;
}

long part_1(char const* fn)
{
    auto ns = parse_input(fn);
    //std::cout << ns << std::endl;

    auto is_part_num = [](num_t const& n, std::vector<symbol_t> const& vs) {
        for (auto const& s : vs)
        {
            if ((n.row - s.row) * (n.row - s.row) >= 2) continue;
            for (auto col : n.cols)
                if ((n.row - s.row) * (n.row - s.row) + (col - s.col) * (col - s.col) <= 2) return true;
        }
        return false;
    };

    long sum = 0;
    for (auto const& n : ns.first)
        if (is_part_num(n, ns.second)) sum += n.value;

    return sum;
}

long part_2(char const* fn)
{
    auto ns = parse_input(fn);

    long sum = 0;
    std::vector<num_t> nums{};
    for (auto const& s : ns.second)
    {
        nums.clear();
        for (auto const& n : ns.first)
        {
            if ((n.row - s.row) * (n.row - s.row) >= 2) continue;
            for (auto col : n.cols)
                if ((n.row - s.row) * (n.row - s.row) + (col - s.col) * (col - s.col) <= 2)
                {
                    nums.push_back(n);
                    break;
                }
        }
        if (nums.size() == 2) sum += nums[0].value * nums[1].value;
    }

    return sum;
}

int main(int argc, char* argv[])
{
    std::cout << "What is the sum of all of the part numbers in the engine schematic?\n"
              << part_1(argv[1]) << std::endl;
    std::cout << "What is the sum of all of the gear ratios in your engine schematic?\n"
              << part_2(argv[1]) << std::endl;

    return 0;
}