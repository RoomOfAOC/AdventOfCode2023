//
// Created by Harold on 2023/12/20.
//

#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <numeric>

#include "../utils/str_op.h"

using mapping_vec_t = std::unordered_map<std::string, std::vector<std::string>>;

struct input_t
{
    mapping_vec_t flip_flops;
    mapping_vec_t conjunctions;
    std::vector<std::string> broadcaster;
};

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << ' ';
    return os;
}

template <typename K, typename V> std::ostream& operator<<(std::ostream& os, std::unordered_map<K, V> const& m)
{
    for (auto const& [k, v] : m)
        os << k << ": " << v << '\n';
    return os;
}

std::ostream& operator<<(std::ostream& os, input_t const& i)
{
    return os << i.flip_flops << '\n' << i.conjunctions << '\n' << i.broadcaster << '\n';
}

input_t parse_input(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    input_t input{};
    while (std::getline(ifs, line))
    {
        auto v = split(line, " -> ");
        if (v[0] == "broadcaster")
            input.broadcaster = split(v[1], ", ");
        else if (v[0][0] == '%')
            input.flip_flops[v[0].substr(1)] = split(v[1], ", ");
        else if (v[0][0] == '&')
            input.conjunctions[v[0].substr(1)] = split(v[1], ", ");
    }
    return input;
}

using state_mapping_t = std::unordered_map<std::string, bool>;
constexpr bool OFF = false;
constexpr bool ON = true;
constexpr bool LOW = false;
constexpr bool HIGH = true;

using conjunction_memory_t = std::unordered_map<std::string, state_mapping_t>;

struct transition_t
{
    std::string from;
    std::string to;
    bool pulse;
};

std::ostream& operator<<(std::ostream& os, transition_t const& s)
{
    return os << "{" << s.from << " -> " << s.to << " - " << std::boolalpha << s.pulse << "}";
}

std::pair<state_mapping_t, conjunction_memory_t> init_state(input_t const& input)
{
    state_mapping_t flip_flop_states{};
    conjunction_memory_t conjunction_memory;
    for (auto const& [k, v] : input.flip_flops)
    {
        flip_flop_states[k] = OFF;
        for (auto const& t : v)
            if (input.conjunctions.find(t) != input.conjunctions.end()) conjunction_memory[t][k] = LOW;
    }
    for (auto const& [k, v] : input.conjunctions)
        for (auto const& t : v)
            if (input.conjunctions.find(t) != input.conjunctions.end()) conjunction_memory[t][k] = LOW;
    return std::make_pair(flip_flop_states, conjunction_memory);
}

size_t part_1(char const* fn)
{
    auto input = parse_input(fn);
    //std::cout << input << '\n';

    auto [flip_flop_states, conjunction_memory] = init_state(input);
    //std::cout << std::boolalpha << flip_flop_states << '\n' << conjunction_memory << '\n';

    size_t cnt_low = 0;
    size_t cnt_high = 0;

    std::deque<transition_t> dq;
    int cnt = 1000;
    while (cnt--)
    {
        dq.push_back({"button", "broadcaster", LOW});
        while (!dq.empty())
        {
            auto [from, to, pulse] = dq.front();
            dq.pop_front();

            if (pulse == LOW)
                cnt_low++;
            else
                cnt_high++;

            if (to == "broadcaster")
                for (auto const& b : input.broadcaster)
                    dq.push_back({to, b, pulse});
            else if (input.flip_flops.find(to) != input.flip_flops.end())
            {
                // HIGH -> nothing happened
                // LOW -> flip
                if (pulse == LOW)
                {
                    flip_flop_states[to] = !flip_flop_states[to];
                    for (auto const& f : input.flip_flops[to])
                        dq.push_back({to, f, flip_flop_states[to]});
                }
            }
            else if (input.conjunctions.find(to) != input.conjunctions.end())
            {
                conjunction_memory[to][from] = pulse;
                // any LOW -> HIGH
                auto out = std::any_of(conjunction_memory[to].cbegin(), conjunction_memory[to].cend(),
                                       [](auto const& p) { return p.second == LOW; });
                for (auto const& c : input.conjunctions[to])
                    dq.push_back({to, c, out});
            }
        }
    }

    return cnt_low * cnt_high;
}

size_t part_2(char const* fn)
{
    auto input = parse_input(fn);
    auto [flip_flop_states, conjunction_memory] = init_state(input);

    // &dn -> rx
    // &dd, &fh, &xp, &fc -> &dn
    // need lcm of above conjunctions
    std::deque<transition_t> dq;
    std::vector<std::pair<std::string, size_t>> record{{"dd", {}}, {"fh", {}}, {"xp", {}}, {"fc", {}}};
    size_t cnt = 0;
    while (true)
    {
        cnt++;

        // lcm of record
        if (auto r = std::reduce(record.cbegin(), record.cend(), size_t{1},
                                 [](size_t x, auto const& p) { return std::lcm(x, p.second); });
            r > 0)
            return r;

        dq.push_back({"button", "broadcaster", LOW});
        while (!dq.empty())
        {
            auto [from, to, pulse] = dq.front();
            dq.pop_front();

            // if lucky
            if (to == "rx" && pulse == LOW) return cnt;

            // update record
            for (auto& [k, v] : record)
                if (k == from && pulse == HIGH) v = cnt;

            if (to == "broadcaster")
                for (auto const& b : input.broadcaster)
                    dq.push_back({to, b, pulse});
            else if (input.flip_flops.find(to) != input.flip_flops.end())
            {
                // HIGH -> nothing happened
                // LOW -> flip
                if (pulse == LOW)
                {
                    flip_flop_states[to] = !flip_flop_states[to];
                    for (auto const& f : input.flip_flops[to])
                        dq.push_back({to, f, flip_flop_states[to]});
                }
            }
            else if (input.conjunctions.find(to) != input.conjunctions.end())
            {
                conjunction_memory[to][from] = pulse;
                // any LOW -> HIGH
                auto out = std::any_of(conjunction_memory[to].cbegin(), conjunction_memory[to].cend(),
                                       [](auto const& p) { return p.second == LOW; });
                for (auto const& c : input.conjunctions[to])
                    dq.push_back({to, c, out});
            }
        }
    }

    return 0;
}

int main(int argc, char* argv[])
{
    std::cout
        << "What do you get if you multiply the total number of low pulses sent by the total number of high pulses sent?\n"
        << part_1(argv[1]) << std::endl;
    std::cout
        << "what is the fewest number of button presses required to deliver a single low pulse to the module named rx?\n"
        << part_2(argv[1]) << std::endl;

    return 0;
}