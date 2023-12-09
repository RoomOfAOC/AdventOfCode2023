//
// Created by Harold on 2023/12/9.
//

#include <fstream>
#include <string>
#include <charconv>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

template <typename T> T parse_num(std::string_view token)
{
    T num{};
    std::from_chars(token.data(), token.data() + token.size(), num);
    return num;
}

template <typename T> std::vector<T> parse_line(std::string_view line)
{
    std::vector<T> nums;
    size_t pos = 0;
    while ((pos = line.find(' ')) != std::string_view::npos)
    {
        auto token = line.substr(0, pos);
        if (!token.empty()) nums.push_back(parse_num<T>(token));
        line = line.substr(pos + 1);
    }
    if (!line.empty()) nums.push_back(parse_num<T>(line));
    return nums;
}

template <typename T> T find_seq_next(std::vector<T> const& seq)
{
    auto seq_cpy = seq;
    T last_diff = 0;
    while (seq_cpy.back() != 0)
    {
        std::adjacent_difference(seq_cpy.begin(), seq_cpy.end(), seq_cpy.begin());
        last_diff += seq_cpy.back();
    }
    return seq.back() + last_diff;
}

template <typename T> T find_seq_prev(std::vector<T> const& seq)
{
    auto seq_cpy = seq;
    T last_diff = 0;
    T sign{1};
    while (seq_cpy.back() != 0)
    {
        std::adjacent_difference(seq_cpy.begin(), seq_cpy.end(), seq_cpy.begin());
        seq_cpy.erase(seq_cpy.begin());
        last_diff += sign * seq_cpy.front();
        sign *= -1;
    }
    return seq.front() - last_diff;
}

long long part_1(char const* fn)
{
    long long sum = 0;
    std::ifstream ifs(fn);
    std::string line;
    while (std::getline(ifs, line))
    {
        auto seq = parse_line<long long>(line);
        sum += find_seq_next(seq);
    }

    return sum;
}

long long part_2(char const* fn)
{
    long long sum = 0;
    std::ifstream ifs(fn);
    std::string line;
    while (std::getline(ifs, line))
    {
        auto seq = parse_line<long long>(line);
        sum += find_seq_prev(seq);
    }

    return sum;
}

int main(int argc, char* argv[])
{
    std::cout << "What is the sum of these extrapolated values?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What is the sum of these extrapolated values?\n" << part_2(argv[1]) << std::endl;

    return 0;
}