#include "math_helpers.hpp"

#include <set>

float Box::GetSurfaceArea() const
{
    return (2 * x * y) + (2 * y * z) + (2 * z * x);
}

float Box::GetSmallestSideArea() const
{
    std::multiset sides = { x, y, z };

    auto it = sides.begin();
    auto first = *it;
    auto second = *(++it);

    return first * second;
}

float Box::GetSmallestPerimeter() const
{
    std::multiset sides = { x, y, z };

    auto it = sides.begin();
    auto first = *it;
    auto second = *(++it);

    return (2 * first) + (2 * second);
}

float Box::GetVolume() const
{
    return x * y * z;
}

bool operator==(const IVec2& a, const IVec2& b)
{
    return a.x == b.x && a.y == b.y;
}

IVec2 operator+(const IVec2& a, const IVec2& b)
{
    return { .x=a.x + b.x, .y=a.y + b.y };
}
