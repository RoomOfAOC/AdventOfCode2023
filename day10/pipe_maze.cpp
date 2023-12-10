//
// Created by Harold on 2023/12/10.
//

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unordered_set>
#include <queue>
#include <regex>

struct pos_t
{
    int x{}, y{};

    bool operator==(pos_t const& other) const { return x == other.x && y == other.y; }
    bool operator!=(pos_t const& other) const { return !(*this == other); }

    struct hasher
    {
        size_t operator()(pos_t const& p) const { return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1); }
    };
};

std::ostream& operator<<(std::ostream& os, pos_t const& p)
{
    os << "[" << p.x << ", " << p.y << "]";
    return os;
}

std::vector<pos_t> go_next(pos_t const& p, char c)
{
    std::vector<pos_t> next;
    next.reserve(2);
    switch (c)
    {
    case '|':
        next.push_back({p.x, p.y - 1});
        next.push_back({p.x, p.y + 1});
        break;
    case '-':
        next.push_back({p.x - 1, p.y});
        next.push_back({p.x + 1, p.y});
        break;
    case 'L':
        next.push_back({p.x + 1, p.y});
        next.push_back({p.x, p.y - 1});
        break;
    case 'J':
        next.push_back({p.x - 1, p.y});
        next.push_back({p.x, p.y - 1});
        break;
    case '7':
        next.push_back({p.x, p.y + 1});
        next.push_back({p.x - 1, p.y});
        break;
    case 'F':
        next.push_back({p.x, p.y + 1});
        next.push_back({p.x + 1, p.y});
        break;

    default:
        break;
    }
    return next;
}

using maze_t = std::vector<std::string>;

bool valid_pos(pos_t const& p, maze_t const& maze)
{
    int rows = maze.size();
    int cols = maze[0].size();
    return (0 <= p.x && p.x < cols) && (0 <= p.y && p.y < rows);
}

char find_start_symbol(pos_t start, maze_t const& maze)
{
    std::vector<pos_t> next;
    next.reserve(2);
    for (auto symbol : {'|', '-', 'L', 'J', '7', 'F'})
    {
        next.clear();
        for (auto n : go_next(start, symbol))
            if (valid_pos(n, maze)) next.push_back(n);
        if (next.size() != 2) continue;
        bool found = true;
        for (auto n : next)
        {
            char c = maze[n.y][n.x];
            if (c == '.')
                found = false;
            else if (auto nn = go_next(n, c); std::find(nn.begin(), nn.end(), start) == nn.end())
                found = false;
        }
        if (found) return symbol;
    }
    return '.';
}

std::pair<maze_t, pos_t> parse_input(char const* fn)
{
    maze_t maze{};
    pos_t start{};
    std::ifstream ifs(fn);
    std::string line;
    int x = 0, y = 0;
    while (std::getline(ifs, line))
    {
        x = 0;
        for (auto c : line)
        {
            if (c == 'S') start = {x, y};
            x++;
        }
        maze.push_back(line);
        y++;
    }
    return std::make_pair(maze, start);
}

struct distance_t
{
    pos_t p;
    int d;

    // BFS
    bool operator<(distance_t const& other) const { return d > other.d; }
};

int part_1(char const* fn)
{
    auto [maze, start] = parse_input(fn);
    maze[start.y][start.x] = find_start_symbol(start, maze);
    std::cout << maze[start.y][start.x] << '\n';

    std::unordered_set<pos_t, pos_t::hasher> visited{};
    visited.insert(start);
    std::priority_queue<distance_t> pq;
    pq.push({start, 0});
    std::vector<int> dist;
    while (!pq.empty())
    {
        auto [p, d] = pq.top();
        pq.pop();
        dist.push_back(d);
        //std::cout << p << ": " << maze[p.y][p.x] << ", " << d << "; ";
        for (auto n : go_next(p, maze[p.y][p.x]))
        {
            if (valid_pos(n, maze) && visited.find(n) == visited.end())
            {
                //std::cout << n << maze[n.y][n.x] << ", " << d + 1 << "; ";
                visited.insert(n);
                pq.push({n, d + 1});
            }
        }
        //std::cout << '\n';
    }

    return *std::max_element(dist.begin(), dist.end());
}

int part_2(char const* fn)
{
    auto [maze, start] = parse_input(fn);
    maze[start.y][start.x] = find_start_symbol(start, maze);

    std::unordered_set<pos_t, pos_t::hasher> visited{};
    visited.insert(start);
    std::priority_queue<distance_t> pq;
    pq.push({start, 0});
    while (!pq.empty())
    {
        auto [p, d] = pq.top();
        pq.pop();
        for (auto n : go_next(p, maze[p.y][p.x]))
        {
            if (valid_pos(n, maze) && visited.find(n) == visited.end())
            {
                visited.insert(n);
                pq.push({n, d + 1});
            }
        }
    }

    // remove pipe not in loop
    for (auto y = 0; y < maze.size(); y++)
        for (auto x = 0; x < maze[0].size(); x++)
        {
            pos_t p{x, y};
            if (visited.find(p) == visited.end()) maze[y][x] = '.';
        }

    // count area
    // TODO: watershed?
    int area = 0;
    //  _
    // | |  |_|
    std::regex h{"F-*7|L-*J"};
    //  _|  |_
    // |      |
    std::regex v{"F-*J|L-*7"};
    for (auto const& r : maze)
    {
        int enclosed = 0;
        auto n_r = std::regex_replace(std::regex_replace(r, h, ""), v, "|");
        //std::cout << n_r << '\n';
        for (auto c : n_r)
            if (c == '|')
                enclosed++;
            else if (c == '.' && enclosed % 2 == 1) // |.||.|
                area++;
    }

    return area;
}

int main(int argc, char* argv[])
{
    std::cout
        << "How many steps along the loop does it take to get from the starting position to the point farthest from the starting position?\n"
        << part_1(argv[1]) << std::endl;
    std::cout << "How many tiles are enclosed by the loop?\n" << part_2(argv[1]) << std::endl;

    return 0;
}