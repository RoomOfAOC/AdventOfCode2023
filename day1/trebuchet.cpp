//
// Created by Harold on 2023/12/1.
//

#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <iostream>

long part_1(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    long sum = 0;
    std::string digits;
    while (std::getline(ifs, line))
    {
        digits.clear();
        for (auto c : line)
            if (std::isdigit(c)) digits += c;
        sum += (digits[0] - '0') * 10 + (digits[digits.size() - 1] - '0');
    }
    return sum;
}

long part_2(char const* fn)
{
    static const std::vector<std::string_view> s_digits{"one", "two",   "three", "four", "five",
                                                        "six", "seven", "eight", "nine"};
    std::ifstream ifs(fn);
    std::string l;
    long sum = 0;
    std::string digits;
    while (std::getline(ifs, l))
    {
        digits.clear();
        std::string_view line(l);
        for (auto i = 0; i < line.size(); i++)
            if (std::isdigit(line[i]))
                digits += line[i];
            else
            {
                for (auto j = 0; j < 9; j++)
                    if (line.substr(i, s_digits[j].size()) == s_digits[j])
                    {
                        digits += ('1' + j);
                        break;
                    }
            }
        sum += (digits[0] - '0') * 10 + (digits[digits.size() - 1] - '0');
    }
    return sum;
}

int main(int argc, char* argv[])
{
    std::cout << "What is the sum of all of the calibration values?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What is the sum of all of the calibration values?\n" << part_2(argv[1]) << std::endl;

    return 0;
}