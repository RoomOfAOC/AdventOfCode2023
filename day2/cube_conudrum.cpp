//
// Created by Harold on 2023/12/2.
//

#include <fstream>
#include <string>
#include <string_view>
#include <charconv>
#include <vector>
#include <algorithm>
#include <iostream>

struct cube_set
{
    int r, g, b;
};
using game = std::pair<int, std::vector<cube_set>>;
std::ostream& operator<<(std::ostream& os, cube_set const& cs)
{
    os << "(" << cs.r << ", " << cs.g << ", " << cs.b << ")";
    return os;
}
std::ostream& operator<<(std::ostream& os, game const& gg)
{
    os << "game: " << gg.first << "[ ";
    for (auto const& cs : gg.second)
        os << cs << " ";
    os << "]";
    return os;
}

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

game parse_game(std::string_view sv)
{
    int id{};
    std::from_chars(sv.data() + sv.find_first_of(' ') + 1, sv.data() + sv.find_first_of(':'), id);
    game gg{id, {}};
    for (auto subset : split(sv.substr(sv.find_first_of(':') + 2), "; "))
    {
        int r{}, g{}, b{};
        for (auto cube : split(subset, ", "))
        {
            auto num_color = split(cube, " ");
            int num{};
            std::from_chars(num_color[0].data(), num_color[0].data() + num_color[0].size(), num);
            if (num_color[1] == "red")
                r = num;
            else if (num_color[1] == "green")
                g = num;
            else if (num_color[1] == "blue")
                b = num;
        }
        gg.second.push_back({r, g, b});
    }
    return gg;
}

long part_1(char const* fn)
{
    constexpr int r = 12, g = 13, b = 14;

    std::ifstream ifs(fn);
    std::string line;
    long sum = 0;

    auto max_rgb = [](game const& gg) {
        int r{}, g{}, b{};
        for (auto const& cs : gg.second)
        {
            r = std::max(cs.r, r);
            g = std::max(cs.g, g);
            b = std::max(cs.b, b);
        }
        return cube_set{r, g, b};
    };
    while (std::getline(ifs, line))
    {
        auto gg = parse_game(line);
        auto [mr, mg, mb] = max_rgb(gg);
        if (mr <= r && mg <= g && mb <= b) sum += gg.first;
    }
    return sum;
}

long part_2(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    long sum = 0;

    auto max_rgb = [](game const& gg) {
        int r{}, g{}, b{};
        for (auto const& cs : gg.second)
        {
            r = std::max(cs.r, r);
            g = std::max(cs.g, g);
            b = std::max(cs.b, b);
        }
        return cube_set{r, g, b};
    };
    while (std::getline(ifs, line))
    {
        auto gg = parse_game(line);
        auto [mr, mg, mb] = max_rgb(gg);
        sum += mr * mg * mb;
    }
    return sum;
}

int main(int argc, char* argv[])
{
    std::cout << "What is the sum of the IDs of those games?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What is the sum of the power of these sets?\n" << part_2(argv[1]) << std::endl;

    return 0;
}