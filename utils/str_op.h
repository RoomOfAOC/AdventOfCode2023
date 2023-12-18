#include <string>
#include <sstream>
#include <string_view>
#include <vector>
#include <charconv>

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(s);
    while (std::getline(ss, token, delimiter))
        tokens.push_back(token);
    return tokens;
}

std::vector<std::string> split(std::string s, std::string delimeter = " ")
{
    std::vector<std::string> tokens{};
    size_t pos = 0;
    while ((pos = s.find(delimeter)) != std::string::npos)
    {
        auto token = s.substr(0, pos);
        if (!token.empty()) tokens.push_back(token);
        s.erase(0, pos + delimeter.size());
    }
    if (!s.empty()) tokens.push_back(s);
    return tokens;
}

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

template <typename T> T parse_num(std::string_view token)
{
    T num{};
    std::from_chars(token.data(), token.data() + token.size(), num);
    return num;
}

template <typename T> T parse_hex_num(std::string_view token)
{
    T num{};
    std::from_chars(token.data(), token.data() + token.size(), num, 16);
    return num;
}