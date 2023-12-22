//
// Created by Harold on 2023/12/22.
//

#include <fstream>
#include <string>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <unordered_map>

#include "../utils/point3d.h"

using point_t = Point3D<int>;
using cube_set_t = std::unordered_set<point_t, point_t::Hasher>;
struct brick_t
{
    point_t e1{}, e2{};
    std::vector<point_t> cubes{};
};
using input_t = std::vector<brick_t>;

brick_t parse_brick(std::string const& s)
{
    brick_t b{};
    sscanf_s(s.data(), "%d, %d, %d~%d, %d, %d", &b.e1.x, &b.e1.y, &b.e1.z, &b.e2.x, &b.e2.y, &b.e2.z);
    for (auto x = b.e1.x; x <= b.e2.x; x++)
        for (auto y = b.e1.y; y <= b.e2.y; y++)
            for (auto z = b.e1.z; z <= b.e2.z; z++)
                b.cubes.emplace_back(x, y, z);
    return b;
}

std::ostream& operator<<(std::ostream& os, brick_t const& b)
{
    return os << "{" << b.e1 << b.e2 << "}";
}

std::ostream& operator<<(std::ostream& os, input_t const& i)
{
    for (auto const& b : i)
        os << b << '\n';
    return os;
}

input_t parse_input(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    input_t input{};
    while (std::getline(ifs, line))
        input.push_back(parse_brick(line));
    return input;
}

std::pair<input_t, unsigned> do_fall(input_t const& in)
{
    input_t out{};
    out.reserve(in.size());
    cube_set_t all{};
    unsigned cnt = 0;
    for (auto i = 0; i < in.size(); i++)
    {
        auto curr = in[i];
        int fallen_idx = -1;
        while (true)
        {
            auto z = std::min_element(curr.cubes.cbegin(), curr.cubes.cend(), [](auto const& p1, auto const& p2) {
                         return p1.z < p2.z;
                     })->z;
            bool can_fall = (z == 1);
            if (!can_fall)
            {
                for (auto const& c : curr.cubes)
                    // has support under it
                    if (c.z == z && all.find(point_t{c.x, c.y, c.z - 1}) != all.end())
                    {
                        can_fall = true;
                        break;
                    }
            }
            if (can_fall)
            {
                out.push_back(curr);
                for (auto const& c : curr.cubes)
                    all.insert(c);
                break;
            }
            else
            {
                for (auto& c : curr.cubes)
                    c.z -= 1;
                fallen_idx = i;
            }
        }
        if (fallen_idx != -1) cnt++;
    }
    return std::make_pair(out, cnt);
}

long part_1(char const* fn)
{
    auto input = parse_input(fn);
    std::sort(input.begin(), input.end(), [](auto const& b1, auto const& b2) { return b1.e1.z < b2.e1.z; });

    auto [out, _] = do_fall(input);
    auto N = out.size();
    unsigned cnt = 0;
    for (auto i = 0; i < N; i++)
    {
        auto o = out;
        // remove one brick
        o.erase(o.begin() + i);
        auto [_, fallen_cnt] = do_fall(o);
        if (fallen_cnt == 0) cnt++;
    }

    return cnt;
}

long part_2(char const* fn)
{
    auto input = parse_input(fn);
    std::sort(input.begin(), input.end(), [](auto const& b1, auto const& b2) { return b1.e1.z < b2.e1.z; });

    auto [out, _] = do_fall(input);
    auto N = out.size();
    unsigned cnt = 0;
    for (auto i = 0; i < N; i++)
    {
        auto o = out;
        // remove one brick
        o.erase(o.begin() + i);
        auto [_, fallen_cnt] = do_fall(o);
        cnt += fallen_cnt;
    }

    return cnt;
}

int main(int argc, char* argv[])
{
    std::cout << "how many bricks could be safely chosen as the one to get disintegrated?\n"
              << part_1(argv[1]) << std::endl;
    std::cout << "What is the sum of the number of other bricks that would fall?\n" << part_2(argv[1]) << std::endl;

    return 0;
}