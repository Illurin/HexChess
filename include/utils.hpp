#pragma once

#include <chrono>
#include <random>

struct Coord
{
    Coord() = default;
    Coord(int x, int y) : x(x), y(y) {}

    bool operator==(const Coord& rhs) const noexcept
    {
        return x == rhs.x && y == rhs.y;
    }

    Coord operator+(const Coord& rhs) const noexcept
    {
        return Coord(x + rhs.x, y + rhs.y);
    }

    Coord& operator+=(const Coord& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

	Coord operator-(const Coord& rhs) const noexcept
    {
        return Coord(x - rhs.x, y - rhs.y);
    }

    Coord& operator-=(const Coord& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    int x{ 0 };
    int y{ 0 };
};

inline std::default_random_engine randomGenerator()
{
    auto seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    return std::default_random_engine(seed);
}