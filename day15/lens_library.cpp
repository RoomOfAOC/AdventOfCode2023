//
// Created by Harold on 2023/12/15.
//

#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <iostream>

#include "../utils/str_op.h"

unsigned hash(std::string_view step)
{
    unsigned hash = 0;
    for (auto c : step)
        hash = (hash + c) * 17 % 256;
    return hash;
}

size_t part_1(char const* fn)
{
    std::ifstream ifs(fn);
    std::string step;
    size_t sum = 0;
    while (std::getline(ifs, step, ','))
        sum += hash(step);
    return sum;
}

using box_t = std::vector<std::pair<std::string, int>>;

size_t part_2(char const* fn)
{
    std::ifstream ifs(fn);
    std::string step;
    size_t sum = 0;
    std::vector<box_t> boxes(256, box_t{});
    while (std::getline(ifs, step, ','))
    {
        std::string_view seq(step);
        auto op_pos = seq.find_first_of("=-");
        auto label = seq.substr(0, op_pos);
        auto& box = boxes[hash(label)];
        if (auto op = seq[op_pos]; op == '-')
            box.erase(std::remove_if(box.begin(), box.end(), [&label](auto const& p) { return p.first == label; }),
                      box.end());
        else
        {
            auto focal_length = parse_num<int>(seq.substr(op_pos + 1));
            if (auto it = std::find_if(box.begin(), box.end(), [&label](auto const& p) { return p.first == label; });
                it != box.end())
                it->second = focal_length;
            else
                box.push_back({std::string(label), focal_length});
        }
    }
    for (size_t i = 0; i < boxes.size(); i++)
        if (!boxes[i].empty())
            for (size_t j = 0; j < boxes[i].size(); j++)
                sum += (i + 1) * (j + 1) * boxes[i][j].second;
    return sum;
}

int main(int argc, char* argv[])
{
    std::cout << "What is the sum of the results?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What is the focusing power of the resulting lens configuration?\n" << part_2(argv[1]) << std::endl;

    return 0;
}