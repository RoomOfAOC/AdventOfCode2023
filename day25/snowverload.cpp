//
// Created by Harold on 2023/12/28.
//

#include <fstream>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "../utils/str_op.h"

using str_set_t = std::unordered_set<std::string>;
using graph_t = std::unordered_map<std::string, str_set_t>;

std::ostream& operator<<(std::ostream& os, str_set_t const& v)
{
    os << "[ ";
    for (auto const& e : v)
        os << e << ' ';
    return os << "]";
}
std::ostream& operator<<(std::ostream& os, graph_t const& v)
{
    os << "{\n";
    for (auto const& [k, v] : v)
        os << k << ": " << v << '\n';
    return os << "}";
}

// https://en.wikipedia.org/wiki/Minimum_cut
size_t part_1(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    graph_t graph;
    while (std::getline(ifs, line))
    {
        auto v = split(line, ": ");
        for (auto const& c : split(v[1]))
        {
            graph[v[0]].insert(c);
            graph[c].insert(v[0]);
        }
    }
    // std::cout << graph <<'\n';

    // init group0 with all components while group1 empty
    str_set_t group0;
    for (auto const& [k, _] : graph)
        group0.insert(k);

    // count connected components not in group0 (aka. in group1)
    auto count_connected_components = [&graph, &group0](std::string const& c) {
        unsigned cnt = 0;
        for (auto const& cc : graph[c])
            if (group0.find(cc) == group0.end()) cnt++;
        return cnt;
    };

    while (true)
    {
        std::vector<std::pair<std::string, unsigned>> cnts(group0.size());
        std::transform(group0.cbegin(), group0.cend(), cnts.begin(),
                       [&](std::string const& c) { return std::make_pair(c, count_connected_components(c)); });
        auto t = std::accumulate(cnts.cbegin(), cnts.cend(), 0u, [](auto s, auto const& p) { return s + p.second; });
        // only has 3 connection remained with group1
        if (t == 3) break;
        // remove component with most connected components
        group0.erase(std::max_element(cnts.cbegin(), cnts.cend(), [](auto const& p1, auto const& p2) {
                         return p1.second < p2.second;
                     })->first);
    }
    auto s0 = group0.size();
    auto s1 = graph.size() - s0;

    return s0 * s1;
}

int main(int argc, char* argv[])
{
    std::cout << "What do you get if you multiply the sizes of these two groups together?\n"
              << part_1(argv[1]) << std::endl;

    return 0;
}