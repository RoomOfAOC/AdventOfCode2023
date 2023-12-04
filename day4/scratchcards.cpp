//
// Created by Harold on 2023/12/4.
//

#include <fstream>
#include <numeric>
#include <string>
#include <string_view>
#include <charconv>
#include <vector>
#include <algorithm>
#include <iostream>

std::vector<std::string_view> split(std::string_view sv, std::string_view delimeter = " ")
{
    std::vector<std::string_view> tokens{};
    size_t pos = 0;
    while ((pos = sv.find(delimeter)) != std::string_view::npos)
    {
        auto token = sv.substr(0, pos);
        if (!token.empty()) tokens.push_back(token);
        sv = sv.substr(pos + delimeter.size());
    }
    if (!sv.empty()) tokens.push_back(sv);
    return tokens;
}

long part_1(char const* fn)
{
    long sum = 0;
    std::ifstream ifs(fn);
    std::string line;
    while (std::getline(ifs, line))
    {
        int i = 1;
        int cnt = 0;
        auto nums = split(split(line, ": ")[1], " | ");
        auto win_num = split(nums[0]);
        auto card_num = split(nums[1]);
        for (auto n : card_num)
            if (std::find(win_num.cbegin(), win_num.cend(), n) != win_num.cend())
            {
                cnt = i;
                i *= 2;
            }
        sum += cnt;
    }

    return sum;
}

long part_2(char const* fn)
{
    long sum = 0;
    std::ifstream ifs(fn);
    std::string line;
    std::vector<int> cnt;
    while (std::getline(ifs, line))
    {
        int i = 0;
        auto nums = split(split(line, ": ")[1], " | ");
        auto win_num = split(nums[0]);
        auto card_num = split(nums[1]);
        for (auto n : card_num)
            if (std::find(win_num.cbegin(), win_num.cend(), n) != win_num.cend()) i++;
        cnt.push_back(i);
    }

    std::vector<int> copies(cnt.size(), 1);
    for (auto i = 0; i < cnt.size(); i++)
        for (auto j = i + 1; j <= i + cnt[i] && j < cnt.size(); j++)
            copies[j] += copies[i];
    return std::accumulate(copies.cbegin(), copies.cend(), 0l);
}

int main(int argc, char* argv[])
{
    std::cout << "How many points are they worth in total?\n" << part_1(argv[1]) << std::endl;
    std::cout << "how many total scratchcards do you end up with?\n" << part_2(argv[1]) << std::endl;

    return 0;
}