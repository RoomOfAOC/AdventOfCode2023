//
// Created by Harold on 2023/12/17.
//

#include <fstream>
#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>

#include "../utils/mat.h"

using map_t = Mat<int>;

struct state_t
{
    Point pos{};
    Point dir{};
    unsigned moves{};
    unsigned heat{};

    bool operator==(state_t const& other) const { return pos == other.pos && dir == other.dir && moves == other.moves; }

    bool operator<(state_t const& other) const { return heat > other.heat; }

    struct hasher
    {
        size_t operator()(state_t const& s) const
        {
            return Point::PointHasher()(s.pos) ^ Point::PointHasher()(s.dir) ^ std::hash<unsigned>()(s.moves);
        }
    };
};

std::ostream& operator<<(std::ostream& os, state_t const& s)
{
    return os << "{" << s.pos << "; " << s.dir << "; " << s.heat << "; " << s.moves << "}";
}

unsigned min_heat(map_t const& m, unsigned min_moves = 1, unsigned max_moves = 3)
{
    Point start{0, 0};
    Point end{(int)m.rows - 1, (int)m.cols - 1};
    std::vector<Point> dirs{{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    std::unordered_set<state_t, state_t::hasher> visited;
    std::priority_queue<state_t> pq;
    pq.push({start, {0, 1}, 0, 0});
    pq.push({start, {1, 0}, 0, 0});
    while (!pq.empty())
    {
        auto state = pq.top();
        pq.pop();

        // Dijkstra's algorithm ensures the first reached is the shortest
        if (state.pos == end) return state.heat;

        if (visited.find(state) != visited.end()) continue;

        visited.insert(state);
        for (auto const& dir : dirs)
        {
            // no turn back
            if (dir.x == -state.dir.x && dir.y == -state.dir.y) continue;

            auto new_moves = dir == state.dir ? state.moves + 1 : 1;
            // no more moves in one dir
            if (new_moves > max_moves) continue;
            // no less moves in one dir
            if (dir != state.dir && state.moves < min_moves) continue;

            auto new_pos = state.pos + dir;
            if (!m.valid_pos(new_pos)) continue;

            state_t new_state{new_pos, dir, new_moves, state.heat + m(new_pos)};
            //std::cout << new_state << '\n';
            if (visited.find(new_state) == visited.end()) pq.push(new_state);
        }
    }

    return 0;
}

unsigned part_1(char const* fn)
{
    map_t m;
    std::ifstream ifs(fn);
    read_all(ifs, m);

    return min_heat(m);
}

unsigned part_2(char const* fn)
{
    map_t m;
    std::ifstream ifs(fn);
    read_all(ifs, m);

    return min_heat(m, 4, 10);
}

int main(int argc, char* argv[])
{
    std::cout << "what is the least heat loss it can incur?\n" << part_1(argv[1]) << std::endl;
    std::cout << "what is the least heat loss it can incur?\n" << part_2(argv[1]) << std::endl;

    return 0;
}