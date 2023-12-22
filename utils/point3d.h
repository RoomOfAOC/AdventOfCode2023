#include <functional>
#include <ostream>

#include "hash.h"

template <typename T> struct Point3D
{
    T x, y, z;

    Point3D() = default;
    Point3D(T i, T j, T k): x(i), y(j), z(k) {}
    Point3D(Point3D const&) = default;
    Point3D& operator=(Point3D const&) = default;
    Point3D(Point3D&&) = default;
    Point3D& operator=(Point3D&&) = default;

    bool operator==(Point3D const& other) const { return x == other.x && y == other.y && z == other.z; }
    bool operator!=(Point3D const& other) const { return !(*this == other); }

    Point3D& operator-=(Point3D const& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    Point3D& operator+=(Point3D const& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    struct Hasher
    {
        size_t operator()(Point3D const& point) const
        {
            return container_hash<std::initializer_list<int>>()({point.x, point.y, point.z});
        }
    };
};

template <typename T> std::ostream& operator<<(std::ostream& os, Point3D<T> const& p)
{
    os << "[" << p.x << ", " << p.y << ", " << p.z << "]";
    return os;
}

template <typename T> Point3D<T> operator+(Point3D<T> const& p1, Point3D<T> const& p2)
{
    return Point3D(p1.x + p2.x, p1.y + p2.y, p1.z + p2.z);
}

template <typename T> Point3D<T> operator-(Point3D<T> const& p1, Point3D<T> const& p2)
{
    return Point3D(p1.x - p2.x, p1.y - p2.y, p1.z - p2.z);
}
