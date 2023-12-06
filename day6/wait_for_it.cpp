//
// Created by Harold on 2023/12/6.
//

#include <fstream>
#include <string>
#include <string_view>
#include <charconv>
#include <vector>
#include <iostream>
#include <cmath>

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

struct race_t
{
    int time, distance;
};

std::vector<race_t> parse_races(char const* fn)
{
    std::ifstream ifs(fn);
    std::string time, distance;
    std::getline(ifs, time);
    std::getline(ifs, distance);
    auto times = split(split(time, ":")[1]);
    auto distances = split(split(distance, ":")[1]);
    std::vector<race_t> races;
    for (auto i = 0; i < times.size(); i++)
    {
        int t{}, d{};
        std::from_chars(times[i].data(), times[i].data() + times[i].size(), t);
        std::from_chars(distances[i].data(), distances[i].data() + distances[i].size(), d);
        races.push_back({t, d});
    }

    return races;
}

std::ostream& operator<<(std::ostream& os, race_t const& r)
{
    os << "{ time: " << r.time << ", distance: " << r.distance << " }";
    return os;
}

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << ' ';
    os << '\n';
    return os;
}

int count_way_to_win(race_t const& r)
{
    constexpr int acc = 1;

    int cnt = 0;
    for (auto t = 1; t < r.time - 1; t++)
        if ((r.time - t) * (acc * t) > r.distance) cnt++;
    return cnt;
}

long part_1(char const* fn)
{
    auto races = parse_races(fn);
    //std::cout << races;

    long res = 1;
    for (auto const& r : races)
        res *= count_way_to_win(r);

    return res;
}

long long part_2(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line, s;
    std::getline(ifs, line);
    for (auto c : line)
        if (std::isdigit(c)) s += c;
    long long time = std::stoll(s);
    s.clear();
    std::getline(ifs, line);
    for (auto c : line)
        if (std::isdigit(c)) s += c;
    long long distance = std::stoll(s);

    // (t - x) * x > d -> parabola
    // zero cross points: (t ± sqrt(t^2 - 4td)) / 2
    auto r = std::sqrt(time * time / 4 - distance);
    auto low = std::ceil(time / 2. - r);
    auto high = std::floor(time / 2. + r);

    return high - low + 1;
}

int main(int argc, char* argv[])
{
    std::cout << "What do you get if you multiply these numbers together?\n" << part_1(argv[1]) << std::endl;
    std::cout << "How many ways can you beat the record in this one much longer race?\n"
              << part_2(argv[1]) << std::endl;

    return 0;
}