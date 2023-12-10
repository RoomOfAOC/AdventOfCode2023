//
// Created by Harold on 2023/12/7.
//

#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <array>
#include <algorithm>

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

struct hand_t
{
    enum type_t
    {
        HighCard = 0,
        OnePair,
        TwoPair,
        ThreeOfAKind,
        FullHouse,
        FourOfAKind,
        FiveOfAKind
    };

    std::string cards;
    int bid;
    type_t type;
};

std::ostream& operator<<(std::ostream& os, hand_t const& h)
{
    os << "{ cards: " << h.cards << ", bid: " << h.bid << ", type: " << h.type << " }";
    return os;
}

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    for (auto const& e : v)
        os << e << '\n';
    return os;
}

size_t part_1(char const* fn)
{
    std::string_view const CARDS = "23456789TJQKA";
    auto get_type = [CARDS](std::string_view cards) {
        std::array<int, 13> cnt{};
        for (auto c : cards)
            cnt[CARDS.find(c)]++;
        std::sort(cnt.begin(), cnt.end(), std::greater<int>());
        hand_t::type_t type;
        if (cnt[0] == 5)
            type = hand_t::type_t::FiveOfAKind;
        else if (cnt[0] == 4)
            type = hand_t::type_t::FourOfAKind;
        else if (cnt[0] == 3)
            type = hand_t::type_t::ThreeOfAKind;
        else if (cnt[0] == 2)
            type = hand_t::type_t::OnePair;
        else
            type = hand_t::type_t::HighCard;
        if (cnt[1] == 2)
            type = type == hand_t::type_t::ThreeOfAKind ? hand_t::type_t::FullHouse : hand_t::type_t::TwoPair;
        return type;
    };

    std::ifstream ifs(fn);
    std::string line;
    std::vector<hand_t> hands;
    while (std::getline(ifs, line))
    {
        auto h = split(line);
        hands.push_back({std::string(h[0]), std::stoi(std::string(h[1])), get_type(h[0])});
    }
    //std::cout << hands;
    std::stable_sort(hands.begin(), hands.end(), [CARDS](hand_t const& left, hand_t const& right) {
        if (left.type < right.type)
            return true;
        else if (left.type == right.type)
            for (auto i = 0; i < 5; i++)
            {
                auto l = CARDS.find(left.cards[i]);
                auto r = CARDS.find(right.cards[i]);
                if (l == r) continue;
                return l < r;
            }
        return false;
    });
    //std::cout << hands;

    size_t res = 0;
    size_t m = 1;
    for (auto const& h : hands)
        res += h.bid * (m++);

    return res;
}

size_t part_2(char const* fn)
{
    auto get_type = [](std::string_view cards_) {
        std::string_view const CARDS = "23456789TQKA";

        int cnt_j = 0;
        std::string cards{};
        cards.reserve(cards_.size());
        for (auto c : cards_)
            if (c == 'J')
                cnt_j++;
            else
                cards += c;

        if (cnt_j >= 4) return hand_t::type_t::FiveOfAKind;

        std::array<int, 12> cnt{};
        for (auto c : cards)
            cnt[CARDS.find(c)]++;
        std::sort(cnt.begin(), cnt.end(), std::greater<int>());
        hand_t::type_t type;
        if (cnt[0] == 5)
            type = hand_t::type_t::FiveOfAKind;
        else if (cnt[0] == 4)
            type = static_cast<hand_t::type_t>(hand_t::type_t::FourOfAKind + cnt_j);
        else if (cnt[0] == 3)
        {
            type = hand_t::type_t::ThreeOfAKind;
            if (cnt_j == 2)
                type = hand_t::type_t::FiveOfAKind;
            else if (cnt_j == 1)
                type = hand_t::type_t::FourOfAKind;
        }
        else if (cnt[0] == 2)
        {
            type = hand_t::type_t::OnePair;
            if (cnt_j == 3)
                type = hand_t::type_t::FiveOfAKind;
            else if (cnt_j == 2)
                type = hand_t::type_t::FourOfAKind;
            else if (cnt_j == 1)
                type = hand_t::type_t::ThreeOfAKind;
        }
        else
        {
            type = hand_t::type_t::HighCard;
            if (cnt_j == 3)
                type = hand_t::type_t::FourOfAKind;
            else if (cnt_j == 2)
                type = hand_t::type_t::ThreeOfAKind;
            else if (cnt_j == 1)
                type = hand_t::type_t::OnePair;
        }
        if (cnt[1] == 2)
            type = type == hand_t::type_t::ThreeOfAKind ? hand_t::type_t::FullHouse : hand_t::type_t::TwoPair;
        return type;
    };

    std::ifstream ifs(fn);
    std::string line;
    std::vector<hand_t> hands;
    while (std::getline(ifs, line))
    {
        auto h = split(line);
        hands.push_back({std::string(h[0]), std::stoi(std::string(h[1])), get_type(h[0])});
    }
    //std::cout << hands;
    std::stable_sort(hands.begin(), hands.end(), [](hand_t const& left, hand_t const& right) {
        std::string_view const CARDS = "J23456789TQKA";

        if (left.type < right.type)
            return true;
        else if (left.type == right.type)
            for (auto i = 0; i < 5; i++)
            {
                auto l = CARDS.find(left.cards[i]);
                auto r = CARDS.find(right.cards[i]);
                if (l == r) continue;
                return l < r;
            }
        return false;
    });
    //std::cout << hands;

    size_t res = 0;
    size_t m = 1;
    for (auto const& h : hands)
        res += h.bid * (m++);

    return res;
}

int main(int argc, char* argv[])
{
    std::cout << "What are the total winnings?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What are the new total winnings?\n" << part_2(argv[1]) << std::endl;

    return 0;
}