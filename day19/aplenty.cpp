//
// Created by Harold on 2023/12/19.
//

#include <fstream>
#include <ostream>
#include <stdio.h>
#include <string_view>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <vector>
#include <variant>
#include <optional>
#include <deque>

#include "../utils/str_op.h"

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << '\n';
    return os;
}

using workflow_t = std::unordered_map<std::string, std::string>;

std::ostream& operator<<(std::ostream& os, workflow_t const& w)
{
    for (auto const& [k, v] : w)
        os << k << ": " << v << '\n';
    return os;
}

struct rating_t
{
    unsigned x{}, m{}, a{}, s{};
};

std::ostream& operator<<(std::ostream& os, rating_t const& r)
{
    return os << "{x=" << r.x << ",m=" << r.m << ",a=" << r.a << ",s=" << r.s << "}";
}

rating_t parse_rating(std::string_view s)
{
    rating_t r;
    sscanf_s(s.data(), "{x=%u,m=%u,a=%u,s=%u}", &r.x, &r.m, &r.a, &r.s);
    return r;
}

unsigned get(rating_t const& r, char c)
{
    switch (c)
    {
    case 'x':
        return r.x;
    case 'm':
        return r.m;
    case 'a':
        return r.a;
    case 's':
        return r.s;
    default:
        return 0;
    }
}

using input_t = std::pair<workflow_t, std::vector<rating_t>>;

std::ostream& operator<<(std::ostream& os, input_t const& i)
{
    return os << i.first << '\n' << i.second;
}

input_t parse_input(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    input_t input{};
    while (std::getline(ifs, line) && !line.empty())
    {
        auto s = line.find('{');
        input.first[line.substr(0, s)] = line.substr(s + 1, line.size() - s - 2);
    }
    while (std::getline(ifs, line))
        input.second.push_back(parse_rating(line));
    return input;
}

bool comp(rating_t const& r, std::string_view condition)
{
    auto a = get(r, condition[0]);
    auto op = condition[1];
    auto b = parse_num<unsigned>(condition.substr(2));
    return (op == '<') ? (a < b) : (a > b);
}

bool is_accepted(rating_t const& r, workflow_t& w)
{
    std::string rule = w["in"];
    auto next_rule = [&r, &w](std::string rule) -> std::variant<bool, std::string> {
        std::string workflow_key;
        for (auto sr : split(rule, ","))
        {
            if (auto p = sr.find(':'); p == decltype(sr)::npos)
                workflow_key = sr;
            else if (comp(r, sr.substr(0, p)))
                workflow_key = sr.substr(p + 1);
            else
                continue;

            if (workflow_key == "A")
                return true;
            else if (workflow_key == "R")
                return false;
            else
                return w[workflow_key];
        }
        throw "should not reach here";
    };
    auto n = next_rule(rule);
    while (n.index() == 1)
        n = next_rule(std::get<std::string>(n));
    return std::get<bool>(n);
}

size_t part_1(char const* fn)
{
    auto input = parse_input(fn);
    //std::cout << input << '\n';

    size_t sum = 0;
    for (auto const& r : input.second)
        if (is_accepted(r, input.first)) sum += r.x + r.m + r.a + r.s;

    return sum;
}

/*-------------------------------------------part-2-------------------------------------------------*/

using rng_t = std::pair<unsigned, unsigned>;

std::ostream& operator<<(std::ostream& os, rng_t const& rng)
{
    return os << "(" << rng.first << ", " << rng.second << ")";
}

struct rating_rng_t
{
    rng_t x{1, 4000};
    rng_t m{1, 4000};
    rng_t a{1, 4000};
    rng_t s{1, 4000};

    rng_t& get(char c)
    {
        switch (c)
        {
        case 'x':
            return x;
        case 'm':
            return m;
        case 'a':
            return a;
        case 's':
            return s;
        default:
            throw "should not reach here";
        }
    }

    rng_t const& get(char c) const
    {
        switch (c)
        {
        case 'x':
            return x;
        case 'm':
            return m;
        case 'a':
            return a;
        case 's':
            return s;
        default:
            throw "should not reach here";
        }
    }

    size_t possibilities()
    {
        return static_cast<size_t>(x.second - x.first + 1) * (m.second - m.first + 1) * (a.second - a.first + 1) *
               (s.second - s.first + 1);
    }
};

std::ostream& operator<<(std::ostream& os, rating_rng_t const& r)
{
    return os << "{x=" << r.x << ",m=" << r.m << ",a=" << r.a << ",s=" << r.s << "}";
}

struct condition_t
{
    char rating{};
    char op{};
    unsigned limit{};
};

condition_t parse_condition(std::string_view condition)
{
    return {condition[0], condition[1], parse_num<unsigned>(condition.substr(2))};
}

std::ostream& operator<<(std::ostream& os, condition_t const& c)
{
    return os << "{" << c.rating << ", " << c.op << ", " << c.limit << "}";
}

struct rule_t
{
    std::optional<condition_t> condition{};
    std::string_view next{};
};

std::vector<rule_t> parse_rules(std::string_view rs)
{
    std::vector<rule_t> rules;
    for (auto r : split(rs, ","))
    {
        rule_t rule;
        if (auto p = r.find(':'); p == decltype(r)::npos)
            rule.next = r;
        else
        {
            rule.condition = parse_condition(r.substr(0, p));
            rule.next = r.substr(p + 1);
        }
        rules.push_back(rule);
    }
    return rules;
}

std::ostream& operator<<(std::ostream& os, rule_t const& r)
{
    return (r.condition ? (os << r.condition.value()) : (os << "nullptr")) << "; " << r.next;
}

std::optional<rng_t> new_rng(rating_rng_t const& rng, condition_t const& condition)
{
    auto [a, op, b] = condition;
    auto [start, end] = rng.get(a);
    if (op == '>' && end > b) return rng_t{b + 1, end};
    if (op == '<' && start < b) return rng_t{start, b - 1};
    return {};
}

#include <queue>

size_t part_2(char const* fn)
{
    auto [workflows, _] = parse_input(fn);
    size_t sum = 0;
    std::deque<std::pair<std::string, rating_rng_t>> dq;
    dq.push_back({"in", rating_rng_t{}});
    while (!dq.empty())
    {
        auto [workflow, rating_rng] = dq.front();
        dq.pop_front();
        auto rules = parse_rules(workflows[workflow]);

        for (auto const& rule : rules)
        {
            auto next_rng = rating_rng;
            if (auto condition = rule.condition; condition)
            {
                auto cv = condition.value();
                if (auto rng_n = new_rng(rating_rng, cv); rng_n)
                {
                    if (cv.op == '<')
                        rating_rng.get(cv.rating).first = cv.limit;
                    else
                        rating_rng.get(cv.rating).second = cv.limit;

                    next_rng.get(condition->rating).first = rng_n->first;
                    next_rng.get(condition->rating).second = rng_n->second;
                }
            }
            if (rule.next == "A")
                sum += next_rng.possibilities();
            else if (rule.next != "R")
                dq.push_back({std::string(rule.next), next_rng});
        }
    }
    return sum;
}

int main(int argc, char* argv[])
{
    std::cout
        << "what do you get if you add together all of the rating numbers for all of the parts that ultimately get accepted?\n"
        << part_1(argv[1]) << std::endl;
    std::cout << "How many distinct combinations of ratings will be accepted by the Elves' workflows?\n"
              << part_2(argv[1]) << std::endl;

    return 0;
}