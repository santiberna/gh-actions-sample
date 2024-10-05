#pragma once
#include <functional>
#include <cstddef>
#include <iostream>
#include "format.hpp"
#include <vector>
#include <optional>
#include <array>

inline int power_of_int(int i, int n)
{
    int out = 1;
    while ((n--) != 0)
    {
        out *= i;
    }
    return out;
}

struct Box
{
    float m_x {}, m_y {}, m_z {};

    [[nodiscard]] float get_volume() const;
    [[nodiscard]] float get_surface_area() const;
    [[nodiscard]] float get_smallest_side_area() const;
    [[nodiscard]] float get_smallest_perimeter() const;
};

struct IVec2
{
    int m_x, m_y;
};

template <>
struct std::hash<IVec2>
{
    size_t operator()(const IVec2& v) const
    {
        return (static_cast<size_t>(v.m_x) << 32) + static_cast<size_t>(v.m_y);
    }
};

bool operator==(const IVec2& a, const IVec2& b);
IVec2 operator+(const IVec2& a, const IVec2& b);

template <typename T, size_t R, size_t C>
class TMat
{
public:
    template <typename, size_t, size_t>
    friend class TMat;
    using ArrayType = std::array<T, C * R>;

    TMat() = default;
    TMat(const std::array<T, R * C>& data)
        : m_data(data)
    {
    }

    template <size_t V>
    TMat<T, R, V> mul(const TMat<T, C, V>& other) const
    {
        using Data = std::array<T, R * V>;
        Data result {};

        for (size_t r = 0; r < R; r++)
        {
            for (size_t v = 0; v < V; v++)
            {
                for (size_t c = 0; c < C; c++)
                {
                    result[(r * V) + v] += at(r, c) * other.At(c, v);
                }
            }
        }

        return { result };
    }

    TMat<T, C, R> transpose() const
    {
        using Data = std::array<T, C * R>;
        Data transposed {};

        for (size_t r = 0; r < R; r++) {
            for (size_t c = 0; c < C; c++)
            {
                transposed[(c * R) + r] = at(r, c);
            }
}

        return { transposed };
    }

    T at(size_t r, size_t c) const { return m_data[(r * C) + c]; }

    void print(std::ostream& stream) const
    {
        for (size_t r = 0; r < R; r++)
        {
            for (size_t c = 0; c < C; c++)
            {
                stream << fmt::format("{:03} ", at(r, c)) << " ";
            }
            stream << "\n";
        }
    }

    [[nodiscard]] consteval size_t get_rows() const { return R; }
    [[nodiscard]] consteval size_t get_collumns() const { return C; }

private:
    ArrayType m_data;
};

template <typename T>
class VariableMatrix
{
public:
    VariableMatrix(size_t width, size_t height)
        : m_width(width)
        , m_height(height)
    {
        m_data.resize(width * height);
    }

    T* at(size_t x, size_t y)
    {
        if (x >= m_width) {
            return nullptr;
}
        if (y >= m_height) {
            return nullptr;
}

        return &m_data.at((y * m_width) + x);
    }

    const T* at(size_t x, size_t y) const
    {
        if (x >= m_width) {
            return nullptr;
}
        if (y >= m_height) {
            return nullptr;
}

        return &m_data.at((y * m_width) + x);
    }

    std::optional<VariableMatrix<T>> mat_mul(const VariableMatrix<T>& other) const
    {
        if (m_width != other.m_height) {
            return std::nullopt;
}
        size_t out_width = other.m_width;
        size_t out_height = m_height;

        VariableMatrix<T> out { out_width, out_height };

        for (size_t i = 0; i < out_height; i++)
        {
            for (size_t j = 0; j < out_width; j++)
            {
                for (size_t k = 0; k < m_width; k++)
                { // aColumn
                    (*out.At(j, i)) += (*At(k, i)) * (*other.At(j, k));
                }
            }
        }

        return out;
    }

    void print()
    {
        for (size_t y = 0; y < m_height; y++)
        {
            for (size_t x = 0; x < m_width; x++)
            {
                std::cout << fmt::format("{:03} ", *At(x, y));
            }
            std::cout << "\n";
        }
    }

    [[nodiscard]] size_t get_collumns() const { return m_width; }
    [[nodiscard]] size_t get_rows() const { return m_height; }

private:
    std::vector<T> m_data;
    size_t m_width, m_height;
};
