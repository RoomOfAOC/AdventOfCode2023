//
// Created by Harold on 2023/12/18.
//

#include <fstream>
#include <algorithm>
#include <iostream>
#include <vector>

#include "../utils/mat.h"
#include "../utils/str_op.h"

template <typename T> void move(T& p, char dir, int dist)
{
    switch (dir)
    {
    case 'U':
        p.x -= dist;
        break;
    case 'D':
        p.x += dist;
        break;
    case 'L':
        p.y -= dist;
        break;
    case 'R':
        p.y += dist;
        break;
    default:
        break;
    }
}

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << ' ';
    return os;
}

// https://en.wikipedia.org/wiki/Shoelace_formula
template <typename T> auto polygon_area(std::vector<T> const& points)
{
    decltype(T::x) left = 0, right = 0;
    for (int i = 0; i < points.size(); ++i)
    {
        int j = (i + 1) % points.size();
        left += points[i].y * points[j].x;
        right += points[j].y * points[i].x;
    }
    return (left > right ? (left - right) : (right - left)) / 2;
}

size_t part_1(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    Point start{0, 0};
    std::vector<Point> points;
    int perimeter = 0;
    while (std::getline(ifs, line))
    {
        std::string_view l(line);
        auto v = split(l);
        auto dir = v[0][0];
        auto dist = parse_num<int>(v[1]);
        perimeter += dist;
        move(start, dir, dist);
        points.push_back(start);
    }
    //std::cout << points << '\n';
    auto area = polygon_area(points);
    // remained small area of square which cut by the edges
    auto rem = perimeter / 2;

    return area + rem + 1;
}

struct Point2
{
    long long x{}, y{};
};

size_t part_2(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    Point2 start{0, 0};
    std::vector<Point2> points;
    int perimeter = 0;
    while (std::getline(ifs, line))
    {
        std::string_view l(line);
        auto v = split(l);
        auto hex = v[2].substr(1, v[2].size() - 2);
        auto dir = hex.back();
        if (dir == '0')
            dir = 'R';
        else if (dir == '1')
            dir = 'D';
        else if (dir == '2')
            dir = 'L';
        else if (dir == '3')
            dir = 'U';
        auto dist = parse_hex_num<long long>(hex.substr(1, hex.size() - 2));
        perimeter += dist;
        move(start, dir, dist);
        points.push_back(start);
    }

    auto area = polygon_area(points);
    auto rem = perimeter / 2;

    // actually it's called Pick's theorem: https://en.wikipedia.org/wiki/Pick%27s_theorem
    // A = i + b / 2 - 1
    // i = A + 1 - b / 2
    // i + b = A + 1 + b / 2

    return area + rem + 1;
}

int main(int argc, char* argv[])
{
    std::cout << "how many cubic meters of lava could it hold?\n" << part_1(argv[1]) << std::endl;
    std::cout << "how many cubic meters of lava could the lagoon hold?\n" << part_2(argv[1]) << std::endl;

    return 0;
}