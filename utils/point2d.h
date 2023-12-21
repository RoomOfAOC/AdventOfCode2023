#include <functional>
#include <ostream>

template <typename T> struct Point2D
{
    T x, y;

    Point2D() = default;
    Point2D(T i, T j): x(i), y(j) {}
    Point2D(Point2D const&) = default;
    Point2D& operator=(Point2D const&) = default;
    Point2D(Point2D&&) = default;
    Point2D& operator=(Point2D&&) = default;

    bool operator==(Point2D const& other) const { return x == other.x && y == other.y; }
    bool operator!=(Point2D const& other) const { return !(*this == other); }

    Point2D& operator-=(Point2D const& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }
    Point2D& operator+=(Point2D const& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    struct Hasher
    {
        size_t operator()(Point2D const& point) const
        {
            return std::hash<T>()(point.x) ^ (std::hash<T>()(point.y) << 1);
        }
    };
};

template <typename T> std::ostream& operator<<(std::ostream& os, Point2D<T> const& p)
{
    os << "[" << p.x << ", " << p.y << "]";
    return os;
}

template <typename T> Point2D<T> operator+(Point2D<T> const& p1, Point2D<T> const& p2)
{
    return Point2D(p1.x + p2.x, p1.y + p2.y);
}

template <typename T> Point2D<T> operator-(Point2D<T> const& p1, Point2D<T> const& p2)
{
    return Point2D(p1.x - p2.x, p1.y - p2.y);
}
