//
// Created by Harold on 2023/12/26.
//

// just back from trip, continue solving these puzzles :)

#include <fstream>
#include <algorithm>
#include <iostream>
#include <ostream>
#include <unordered_set>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../utils/mat.h"

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    os << "[ ";
    for (auto const& e : v)
        os << e << ' ';
    return os << "]";
}

template <typename T, typename U> std::ostream& operator<<(std::ostream& os, std::unordered_set<T, U> const& v)
{
    os << "[ ";
    for (auto const& e : v)
        os << e << ' ';
    return os << "]";
}

using map_t = Mat<char>;
using point_vec_t = std::vector<Point>;
using point_set_t = std::unordered_set<Point, Point::PointHasher>;
using neighbors_map_t = std::unordered_map<Point, point_set_t, Point::PointHasher>;
struct state_t
{
    Point pos{};
    int dist{};

    bool operator==(state_t const& other) const { return pos == other.pos && dist == other.dist; }

    struct Hasher
    {
        size_t operator()(state_t const& s) const
        {
            return Point::PointHasher()(s.pos) ^ (std::hash<int>()(s.dist) << 1);
        }
    };
};
using state_vec_t = std::vector<state_t>;
using state_set_t = std::unordered_set<state_t, state_t::Hasher>;

std::ostream& operator<<(std::ostream& os, state_t const& s)
{
    return os << "{" << s.pos << ", " << s.dist << "}";
}

int part_1(char const* fn)
{
    std::ifstream ifs(fn);
    map_t m;
    read_all(ifs, m);
    Point start{}, end{};
    int R = m.rows;
    int C = m.cols;
    for (auto c = 0; c < C; c++)
    {
        if (m(0, c) == '.') start = {0, c};
        if (m(R - 1, c) == '.') end = {R - 1, c};
    }
    point_vec_t neighbors;
    neighbors.reserve(4);
    neighbors_map_t neighbors_map;
    for (auto r = 0; r < R; r++)
        for (auto c = 0; c < C; c++)
            if (m(r, c) != '#')
            {
                Point p{r, c};
                if (m(p) == '.')
                {
                    neighbors.clear();
                    m.find_4_neighbors(p, neighbors);
                    for (auto const& n : neighbors)
                        if (m(n) == '.')
                        {
                            neighbors_map[p].insert(n);
                            neighbors_map[n].insert(p);
                        }
                }
                else
                {
                    Point next{-1, -1};
                    Point prev{-1, -1};
                    if (m(p) == '>')
                    {
                        next = {r, c + 1};
                        prev = {r, c - 1};
                    }
                    else if (m(p) == '<')
                    {
                        next = {r, c - 1};
                        prev = {r, c + 1};
                    }
                    else if (m(p) == '^')
                    {
                        next = {r - 1, c};
                        prev = {r + 1, c};
                    }
                    else if (m(p) == 'v')
                    {
                        next = {r + 1, c};
                        prev = {r - 1, c};
                    }

                    if (m.valid_pos(next) && m(next) != '#') neighbors_map[p].insert(next);
                    if (m.valid_pos(prev) && m(prev) != '#') neighbors_map[prev].insert(p);
                }
            }
    // for (auto const& [k, v] : neighbors_map)
    //     std::cout << k << ": " << v << '\n';
    // dfs
    point_set_t visited{};
    state_vec_t stack;
    stack.push_back({start, 0});
    int max = 0;
    while (!stack.empty())
    {
        auto state = stack.back();
        stack.pop_back();
        // std::cout << state << '\n';
        if (state.dist == -1)
        {
            visited.erase(state.pos);
            continue;
        }
        if (state.pos == end)
        {
            max = std::max(max, state.dist);
            // std::cout << max << '\n';
            continue;
        }
        if (visited.find(state.pos) == visited.end())
        {
            visited.insert(state.pos);
            stack.push_back({state.pos, -1});
            for (auto const& n : neighbors_map[state.pos])
                stack.push_back({n, state.dist + 1});
        }
    }
    return max;
}

using neighbor_dist_map_t = std::unordered_map<Point, state_set_t, Point::PointHasher>;
int part_2(char const* fn)
{
    std::ifstream ifs(fn);
    map_t m;
    read_all(ifs, m);
    Point start{}, end{};
    int R = m.rows;
    int C = m.cols;
    for (auto c = 0; c < C; c++)
    {
        if (m(0, c) == '.') start = {0, c};
        if (m(R - 1, c) == '.') end = {R - 1, c};
    }
    point_vec_t neighbors;
    neighbors.reserve(4);
    neighbor_dist_map_t neighbors_map;
    for (auto r = 0; r < R; r++)
        for (auto c = 0; c < C; c++)
            if (m(r, c) != '#')
            {
                Point p{r, c};
                neighbors.clear();
                m.find_4_neighbors(p, neighbors);
                for (auto const& n : neighbors)
                    if (m(n) != '#')
                    {
                        neighbors_map[p].insert({n, 1});
                        neighbors_map[n].insert({p, 1});
                    }
            }
    // eliminate points with 2 neighbors (`n1-p-n2`) and accumulate the path length from one multi-paths point to another
    // aka. [edge contraction](https://en.wikipedia.org/wiki/Edge_contraction)
    while (true)
    {
        bool merged = false;
        for (auto const& [p, nm] : neighbors_map)
            if (nm.size() == 2)
            {
                auto it = nm.cbegin();
                auto left = *(it++);
                auto right = *it;
                neighbors_map[left.pos].erase({p, left.dist});
                neighbors_map[right.pos].erase({p, right.dist});
                neighbors_map[left.pos].insert({right.pos, left.dist + right.dist});
                neighbors_map[right.pos].insert({left.pos, left.dist + right.dist});
                neighbors_map.erase(p);
                merged = true;
                break;
            }
        if (!merged) break;
    }
    // for (auto const& [k, v] : neighbors_map)
    //     std::cout << k << ": " << v << '\n';
    // dfs
    point_set_t visited{};
    state_vec_t stack;
    stack.push_back({start, 0});
    int max = 0;
    while (!stack.empty())
    {
        auto state = stack.back();
        stack.pop_back();
        // std::cout << state << '\n';
        if (state.dist == -1)
        {
            visited.erase(state.pos);
            continue;
        }
        if (state.pos == end)
        {
            max = std::max(max, state.dist);
            continue;
        }
        if (visited.find(state.pos) == visited.end())
        {
            visited.insert(state.pos);
            stack.push_back({state.pos, -1});
            for (auto const& [n, dist] : neighbors_map[state.pos])
                stack.push_back({n, state.dist + dist});
        }
    }
    return max;
}

int main(int argc, char* argv[])
{
    std::cout << "How many steps long is the longest hike?\n" << part_1(argv[1]) << std::endl;
    std::cout << "How many steps long is the longest hike?\n" << part_2(argv[1]) << std::endl;

    return 0;
}