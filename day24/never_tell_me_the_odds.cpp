//
// Created by Harold on 2023/12/27.
//

#include <fstream>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <optional>

#include "../utils/str_op.h"
#include "../utils/point2d.h"
#include "../utils/point3d.h"

#include <z3++.h>

template <typename T> std::ostream& operator<<(std::ostream& os, std::vector<T> const& v)
{
    os << "[ ";
    for (auto const& e : v)
        os << e << ' ';
    return os << "]";
}

using point2d_t = Point2D<long long>;
struct ray2d
{
    point2d_t p{}, k{};
};
std::ostream& operator<<(std::ostream& os, ray2d const& ray)
{
    return os << "{" << ray.p << ", " << ray.k << "}";
}
using ray2d_vec_t = std::vector<ray2d>;
using point2d_d_t = Point2D<double>;
std::optional<point2d_d_t> intersect(ray2d const& ray1, ray2d const& ray2)
{
    if (ray1.p == ray2.p) return point2d_d_t(ray1.p.x, ray1.p.y);
    auto det = ray2.k.x * ray1.k.y - ray2.k.y * ray1.k.x;
    if (det != 0)
    {
        auto dx = ray2.p.x - ray1.p.x;
        auto dy = ray2.p.y - ray1.p.y;
        auto u = (dy * ray2.k.x - dx * ray2.k.y) / static_cast<double>(det);
        auto v = (dy * ray1.k.x - dx * ray1.k.y) / static_cast<double>(det);
        if (u >= 0 && v >= 0) return point2d_d_t{ray1.k.x * u + ray1.p.x, ray1.k.y * u + ray1.p.y};
    }
    return {};
}

size_t part_1(char const* fn)
{
    constexpr long long MIN = 200000000000000;
    constexpr long long MAX = 400000000000000;

    std::ifstream ifs(fn);
    std::string line;
    ray2d_vec_t rays;
    while (std::getline(ifs, line))
    {
        ray2d ray;
        sscanf_s(line.data(), "%lld, %lld, %*lld @ %lld, %lld, %*lld", &ray.p.x, &ray.p.y, &ray.k.x, &ray.k.y);
        rays.push_back(ray);
    }
    // std::cout << rays <<'\n';
    size_t cnt = 0;
    for (auto i = 0; i < rays.size(); i++)
        for (auto j = i + 1; j < rays.size(); j++)
            if (auto inter_p = intersect(rays[i], rays[j]);
                inter_p && inter_p->x >= MIN && inter_p->x <= MAX && inter_p->y >= MIN && inter_p->y <= MAX)
                cnt++;

    return cnt;
}

using point3d_t = Point3D<long long>;
struct ray3d
{
    point3d_t p{}, k{};
};
std::ostream& operator<<(std::ostream& os, ray3d const& ray)
{
    return os << "{" << ray.p << ", " << ray.k << "}";
}
using ray3d_vec_t = std::vector<ray3d>;
long long part_2(char const* fn)
{
    std::ifstream ifs(fn);
    std::string line;
    ray3d_vec_t rays;
    while (std::getline(ifs, line))
    {
        ray3d ray;
        sscanf_s(line.data(), "%lld, %lld, %lld @ %lld, %lld, %lld", &ray.p.x, &ray.p.y, &ray.p.z, &ray.k.x, &ray.k.y,
                 &ray.k.z);
        rays.push_back(ray);
    }
    // std::cout << rays <<'\n';
    // p0 + k0 * ti = pi + ki * ti  ->  (p0 - pi) = -ti * (k0 - ki)  ->  (p0 - pi) @ (k0 - ki) = 0
    // gaussian elimination? int double rounding?
    // use Z3 is simpler...
    // https://github.com/Z3Prover/z3/blob/master/examples/c%2B%2B/example.cpp
    z3::context context{};
    auto px = context.int_const("px");
    auto py = context.int_const("py");
    auto pz = context.int_const("pz");
    auto kx = context.int_const("kx");
    auto ky = context.int_const("ky");
    auto kz = context.int_const("kz");
    z3::solver solver(context);

    for (auto i = 0; i < rays.size(); i++)
    {
        auto const& ray = rays[i];
        auto pi_x = context.int_val(ray.p.x);
        auto pi_y = context.int_val(ray.p.y);
        auto pi_z = context.int_val(ray.p.z);
        auto ki_x = context.int_val(ray.k.x);
        auto ki_y = context.int_val(ray.k.y);
        auto ki_z = context.int_val(ray.k.z);
        auto ti = context.int_const(("t" + std::to_string(i)).c_str());

        solver.add(px + kx * ti == pi_x + ki_x * ti);
        solver.add(py + ky * ti == pi_y + ki_y * ti);
        solver.add(pz + kz * ti == pi_z + ki_z * ti);
    }
    solver.check();
    auto model = solver.get_model();
    return model.eval(px + py + pz).as_int64();
}

int main(int argc, char* argv[])
{
    std::cout << "How many of these intersections occur within the test area?\n" << part_1(argv[1]) << std::endl;
    std::cout << "What do you get if you add up the X, Y, and Z coordinates of that initial position?\n"
              << part_2(argv[1]) << std::endl;

    return 0;
}