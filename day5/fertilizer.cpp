//
// Created by Harold on 2023/12/5.
//

#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <iostream>

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

struct mapping
{
    long long dest_start, src_start, length;
};

std::vector<mapping> parse_mapping(std::ifstream& ifs, std::string const& name)
{
    std::vector<mapping> map;
    std::string line;
    while (std::getline(ifs, line) && line.find(name) == std::string::npos)
        ;
    while (std::getline(ifs, line) && !line.empty())
    {
        std::stringstream ss(line);
        mapping m{};
        ss >> m.dest_start >> m.src_start >> m.length;
        map.push_back(m);
    }
    return map;
}

std::pair<bool, long long> convert(long long in, mapping const& m)
{
    if (in < m.src_start || in >= m.src_start + m.length) return {false, in};
    return {true, in - m.src_start + m.dest_start};
}

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << ' ';
    os << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, std::vector<mapping> const& m)
{
    for (auto const& mm : m)
        os << mm.dest_start << " " << mm.src_start << " " << mm.length << '\n';
    return os;
}

long long part_1(char const* fn)
{
    static const std::vector<std::string> maps = {
        "seed-to-soil map:",         "soil-to-fertilizer map:",     "fertilizer-to-water map:", "water-to-light map:",
        "light-to-temperature map:", "temperature-to-humidity map", "humidity-to-location map:"};

    std::ifstream ifs(fn);
    std::string line;
    std::getline(ifs, line);
    std::vector<long long> seeds;
    for (auto s : split(split(line, ": ")[1]))
        seeds.emplace_back(std::stoll(std::string(s)));
    //std::cout << seeds;
    std::vector<std::vector<mapping>> process;
    for (auto const& n : maps)
        process.push_back(parse_mapping(ifs, n));

    for (auto& seed : seeds)
        for (auto const& p : process)
            for (auto const& m : p)
            {
                auto [ok, out] = convert(seed, m);
                seed = out;
                if (ok) break;
            }
    //std::cout << seeds;

    return *std::min_element(seeds.cbegin(), seeds.cend());
}

long long part_2(char const* fn)
{
    static const std::vector<std::string> maps = {
        "seed-to-soil map:",         "soil-to-fertilizer map:",     "fertilizer-to-water map:", "water-to-light map:",
        "light-to-temperature map:", "temperature-to-humidity map", "humidity-to-location map:"};

    std::ifstream ifs(fn);
    std::string line;
    std::getline(ifs, line);
    std::vector<long long> seeds;
    for (auto s : split(split(line, ": ")[1]))
        seeds.emplace_back(std::stoll(std::string(s)));
    std::vector<std::pair<long long, long long>> seed_pairs;
    for (auto i = 0; i < seeds.size(); i += 2)
        seed_pairs.emplace_back(std::make_pair(seeds[i], seeds[i] + seeds[i + 1] - 1));
    std::vector<std::vector<mapping>> process;
    for (auto const& n : maps)
        process.push_back(parse_mapping(ifs, n));
    seeds.clear();

    for (auto pair : seed_pairs)
    {
        decltype(seed_pairs) unconverted{pair};
        decltype(seed_pairs) converted{};
        for (auto const& p : process)
        {
            while (!unconverted.empty())
            {
                auto [start, end] = unconverted.back();
                unconverted.pop_back();

                bool do_convert = false;
                for (auto const& m : p)
                {
                    // no convert ()[] or []()
                    if (end < m.src_start || start >= m.src_start + m.length) continue;
                    // fully inside [()]
                    else if (start >= m.src_start && end < m.src_start + m.length)
                    {
                        converted.push_back({m.dest_start + start - m.src_start, m.dest_start + end - m.src_start});
                        do_convert = true;
                        break;
                    }
                    // overlap ([)]
                    else if (start < m.src_start && end >= m.src_start && end < m.src_start + m.length)
                    {
                        converted.push_back({m.dest_start, m.dest_start + end - m.src_start});
                        unconverted.push_back({start, m.src_start - 1});
                        do_convert = true;
                        break;
                    }
                    // overlap [(])
                    else if (start >= m.src_start && start < m.src_start + m.length && end >= m.src_start + m.length)
                    {
                        converted.push_back({m.dest_start + start - m.src_start, m.dest_start + m.length - 1});
                        unconverted.push_back({m.src_start + m.length, end});
                        do_convert = true;
                        break;
                    }
                    // fully encompass ([])
                    else if (start < m.src_start && end >= m.src_start + m.length)
                    {
                        converted.push_back({m.dest_start, m.dest_start + m.length - 1});
                        unconverted.push_back({start, m.src_start - 1});
                        unconverted.push_back({m.src_start + m.length, end});
                        do_convert = true;
                        break;
                    }
                }
                // no need to convert
                if (!do_convert) converted.push_back({start, end});
            }
            // go to next process
            unconverted = converted;
            converted.clear();
        }
        // record min
        for (auto const& p : unconverted)
            seeds.push_back(p.first);
    }

    return *std::min_element(seeds.cbegin(), seeds.cend());
}

int main(int argc, char* argv[])
{
    std::cout << "What is the lowest location number that corresponds to any of the initial seed numbers?\n"
              << part_1(argv[1]) << std::endl;
    std::cout << "What is the lowest location number that corresponds to any of the initial seed numbers?\n"
              << part_2(argv[1]) << std::endl;

    return 0;
}