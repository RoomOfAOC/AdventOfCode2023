#include <functional>

template <typename T, typename... Rest> void hash_combine(std::size_t& seed, const T& v, const Rest&... rest)
{
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hash_combine(seed, rest), ...);
}

template <typename T> struct container_hash
{
    template <typename U> static std::size_t make_hash(const U& v) { return std::hash<U>()(v); }

    static void hash_combine(std::size_t& h, const std::size_t& v) { h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2); }

    size_t operator()(const T& v) const
    {
        size_t h = 0;
        for (auto const& e : v)
            hash_combine(h, make_hash(e));
        return h;
    }
};