//
// Created by Harold on 2023/12/21.
//

#include <fstream>
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>

#include "../utils/mat.h"

using map_t = Mat<char>;

struct state_t
{
    Point pos{};
    unsigned moves{};

    bool operator==(state_t const& other) const { return pos == other.pos && moves == other.moves; }

    bool operator<(state_t const& other) const { return moves > other.moves; }

    struct hasher
    {
        size_t operator()(state_t const& s) const
        {
            return Point::PointHasher()(s.pos) ^ std::hash<unsigned>()(s.moves);
        }
    };
};

size_t part_1(char const* fn)
{
    std::ifstream ifs(fn);
    map_t m;
    read_all(ifs, m);
    Point start{};
    for (auto r = 0; r < m.rows; r++)
        for (auto c = 0; c < m.cols; c++)
            if (m(r, c) == 'S')
            {
                start = {r, c};
                m(start) = '0';
                break;
            }

    std::priority_queue<state_t> path;
    path.push({start, 0});
    std::unordered_set<state_t, state_t::hasher> visited;
    std::vector<Point> neighbors;
    neighbors.reserve(4);
    while (!path.empty())
    {
        auto p = path.top();
        path.pop();

        if (visited.find(p) != visited.end()) continue;

        if (p.moves == 64)
        {
            //std::cout << m << '\n';
            unsigned cnt = 0;
            for (auto r = 0; r < m.rows; r++)
                for (auto c = 0; c < m.cols; c++)
                    if (auto cc = m(r, c); cc != '.' && cc != '#' && (cc - '0') % 2 == 0) cnt++;
            return cnt;
        }

        visited.insert(p);

        neighbors.clear();
        m.find_4_neighbors(p.pos, neighbors);
        for (auto const& n : neighbors)
            if (m(n) == '.')
            {
                m(n) = '1' + p.moves;
                path.push({n, p.moves + 1});
            }
    }

    return 0;
}

#include "../utils/point2d.h"
#include <unordered_map>

using point_t = Point2D<long long>;

char get(point_t const& p, map_t const& m)
{
    auto [r, c] = p;
    long long N = m.rows;
    r = (r % N + N) % N;
    c = (c % N + N) % N;
    return m(r, c);
}

size_t part_2(char const* fn)
{
    constexpr unsigned long STEPS = 26501365;

    std::ifstream ifs(fn);
    map_t m;
    read_all(ifs, m);
    point_t start{};
    for (auto r = 0; r < m.rows; r++)
        for (auto c = 0; c < m.cols; c++)
            if (m(r, c) == 'S')
            {
                start = {r, c};
                m(r, c) = '.';
                break;
            }

    auto N = m.rows; // square
    //std::cout << STEPS % N << ", " << STEPS % (2 * N) << ", " << STEPS / N << '\n';  // 65, 202300
    auto required_cycles = STEPS / (2 * N) - 1; // 2 N -> 4 squares, should be enough for detecting period

    std::unordered_map<point_t, unsigned long, point_t::Hasher> visited;
    std::vector<point_t> current;
    current.push_back(start);
    visited[start] = 0;
    std::vector<point_t> dirs{{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
    unsigned long steps = 0;
    size_t plots = 0;
    std::vector<size_t> cnts, diff, diff2;
    while (steps < STEPS)
    {
        steps++;
        std::vector<point_t> next;
        for (auto const& p : current)
            for (auto const& d : dirs)
            {
                auto pos = p + d;
                if (visited.find(pos) == visited.end() && get(pos, m) == '.')
                {
                    next.push_back(pos);
                    visited[pos] = steps;
                }
            }
        if (steps % 2 == 1)
        {
            plots += next.size();
            if (steps % (2 * N) == STEPS % N)
            {
                cnts.push_back(plots);
                if (auto sz = cnts.size(); sz > 1) diff.push_back(cnts[sz - 1] - cnts[sz - 2]);
                if (auto sz = diff.size(); sz > 1) diff2.push_back(diff[sz - 1] - diff[sz - 2]);
                if (!diff2.empty()) break;
            }
        }
        current = next;
    }

    auto cycles = steps / (2 * N) - 1;
    auto diff_cycles = required_cycles - cycles;
    auto diff2_cycles = (required_cycles * (required_cycles + 1)) / 2 - (cycles * (cycles + 1)) / 2;
    return diff2.front() * diff2_cycles + diff.front() * diff_cycles + plots;
}

int main(int argc, char* argv[])
{
    std::cout << "how many garden plots could the Elf reach in exactly 64 steps?\n" << part_1(argv[1]) << std::endl;
    std::cout << "how many garden plots could the Elf reach in exactly 26501365 steps?\n"
              << part_2(argv[1]) << std::endl;

    return 0;
}