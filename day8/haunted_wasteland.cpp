//
// Created by Harold on 2023/12/8.
//

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <numeric>

size_t part_1(char const* fn)
{
    std::string const start = "AAA", end = "ZZZ";
    std::ifstream ifs(fn);
    std::string instructions;
    std::getline(ifs, instructions);
    std::string line;
    std::unordered_map<std::string, std::pair<std::string, std::string>> map;
    std::getline(ifs, line);
    while (std::getline(ifs, line))
        map[line.substr(0, 3)] = std::make_pair(line.substr(7, 3), line.substr(12, 3));

    size_t steps = 0;
    std::string current = start;
    while (true)
        for (auto i : instructions)
        {
            current = i == 'L' ? map[current].first : map[current].second;
            steps++;

            if (current == end) return steps;
        }

    return steps;
}

size_t part_2(char const* fn)
{
    std::ifstream ifs(fn);
    std::string instructions;
    std::getline(ifs, instructions);
    std::string line;
    std::unordered_map<std::string, std::pair<std::string, std::string>> map;
    std::getline(ifs, line);
    std::vector<std::string> start, end;
    while (std::getline(ifs, line))
    {
        auto pos = line.substr(0, 3);
        if (line[2] == 'A')
            start.push_back(pos);
        else if (line[2] == 'Z')
            end.push_back(pos);
        map[pos] = std::make_pair(line.substr(7, 3), line.substr(12, 3));
    }

    std::unordered_map<std::string, std::pair<std::string, size_t>> start_2_end;
    std::vector<std::pair<std::string, std::string>> current; // start - current
    current.reserve(start.size());
    for (auto const& s : start)
        current.push_back(std::make_pair(s, s));

    size_t steps = 0;
    while (true)
    {
        for (auto i : instructions)
        {
            // found all starting pos' ending pos
            if (start_2_end.size() == start.size())
                return std::reduce(start_2_end.cbegin(), start_2_end.cend(), start_2_end.cbegin()->second.second,
                                   [](size_t x, auto const& e) { return std::lcm(x, e.second.second); });

            steps++;

            for (auto& [s, c] : current)
            {
                c = i == 'L' ? map[c].first : map[c].second;
                if (c[c.size() - 1] == 'Z' && start_2_end.find(s) == start_2_end.end())
                    start_2_end[s] = std::make_pair(c, steps);
            }
        }
    }

    return steps;
}

int main(int argc, char* argv[])
{
    std::cout << "How many steps are required to reach ZZZ?\n" << part_1(argv[1]) << std::endl;
    std::cout << "How many steps does it take before you're only on nodes that end with Z?\n"
              << part_2(argv[1]) << std::endl;

    return 0;
}