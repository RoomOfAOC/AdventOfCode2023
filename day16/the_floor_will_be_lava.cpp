//
// Created by Harold on 2023/12/16.
//

#include <fstream>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>

#include "../utils/mat.h"

using map_t = Mat<char>;

struct beam_t
{
    Point pos{};
    Point dir{};

    bool operator==(beam_t const& other) const { return pos == other.pos && dir == other.dir; }

    struct hasher
    {
        size_t operator()(beam_t const& b) const { return Point::PointHasher()(b.pos) ^ Point::PointHasher()(b.dir); }
    };
};

std::ostream& operator<<(std::ostream& os, beam_t const& b)
{
    return os << "{" << b.pos << "; " << b.dir << "}";
}

using visited_t = std::unordered_set<beam_t, beam_t::hasher>;
using tiles_t = std::unordered_set<Point, Point::PointHasher>;
using beams_t = std::queue<beam_t>;

unsigned count_tiles(beam_t const& start, map_t const& m)
{
    visited_t visited;
    tiles_t tiles;
    beams_t beams;
    beams.push(start);
    while (!beams.empty())
    {
        auto beam = beams.front();
        beams.pop();

        if (visited.find(beam) != visited.end()) continue;

        auto [pos, dir] = beam;
        pos += dir;

        visited.insert(beam);
        tiles.insert(beam.pos);

        if (!m.valid_pos(pos.x, pos.y)) continue;

        //std::cout << beam << "; " << pos << '\n';
        switch (m(pos))
        {
        case '/':
            dir = {-dir.y, -dir.x};
            break;
        case '\\':
            dir = {dir.y, dir.x};
            break;
        case '-':
            if (dir.x != 0)
            {
                dir.x = 0;
                dir.y = 1;
                beams.push({pos, {0, -1}});
            }
            break;
        case '|':
            if (dir.y != 0)
            {
                dir.x = 1;
                dir.y = 0;
                beams.push({pos, {-1, 0}});
            }
            break;
        default:
            break;
        }
        beams.push({pos, dir});
    }

    return tiles.size() - 1;
}

unsigned part_1(char const* fn)
{
    map_t m;
    std::ifstream ifs(fn);
    read_all(ifs, m);

    return count_tiles(beam_t{{0, -1}, {0, 1}}, m);
}

unsigned part_2(char const* fn)
{
    map_t m;
    std::ifstream ifs(fn);
    read_all(ifs, m);

    int r = m.rows;
    int c = m.cols;
    unsigned max = 0;
    for (int i = 0; i < c; i++)
        max = std::max(max, std::max(count_tiles(beam_t{{-1, i}, {1, 0}}, m), count_tiles(beam_t{{r, i}, {-1, 0}}, m)));
    for (int i = 0; i < r; i++)
        max = std::max(max, std::max(count_tiles(beam_t{{i, -1}, {0, 1}}, m), count_tiles(beam_t{{i, c}, {0, -1}}, m)));

    return max;
}

int main(int argc, char* argv[])
{
    std::cout << "how many tiles end up being energized?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What is the focusing power of the resulting lens configuration?\n" << part_2(argv[1]) << std::endl;

    return 0;
}