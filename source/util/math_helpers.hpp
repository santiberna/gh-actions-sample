#pragma once
#include <iostream>
#include "format.hpp"
#include <vector>
#include <optional>

inline int PowerOfInt(int i, int n)
{
    int out = 1;
    while (n--)
    {
        out *= i;
    }
    return out;
}

struct Box
{
    float x {}, y {}, z {};

    float GetVolume() const;
    float GetSurfaceArea() const;
    float GetSmallestSideArea() const;
    float GetSmallestPerimeter() const;
};

struct IVec2
{
    int x, y;
};

template <>
struct std::hash<IVec2>
{
    size_t operator()(const IVec2& v) const
    {
        return (static_cast<size_t>(v.x) << 32) + static_cast<size_t>(v.y);
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
        : data(data)
    {
    }

    template <size_t V>
    TMat<T, R, V> Mul(const TMat<T, C, V>& other) const
    {
        using Data = std::array<T, R * V>;
        Data result {};

        for (size_t r = 0; r < R; r++)
        {
            for (size_t v = 0; v < V; v++)
            {
                for (size_t c = 0; c < C; c++)
                {
                    result[r * V + v] += At(r, c) * other.At(c, v);
                }
            }
        }

        return { result };
    }

    TMat<T, C, R> Transpose() const
    {
        using Data = std::array<T, C * R>;
        Data transposed {};

        for (size_t r = 0; r < R; r++)
            for (size_t c = 0; c < C; c++)
            {
                transposed[c * R + r] = At(r, c);
            }

        return { transposed };
    }

    T At(size_t r, size_t c) const { return data[r * C + c]; }

    void Print(std::ostream& stream) const
    {
        for (size_t r = 0; r < R; r++)
        {
            for (size_t c = 0; c < C; c++)
            {
                stream << fmt::format("{:03} ", At(r, c)) << " ";
            }
            stream << "\n";
        }
    }

    consteval size_t GetRows() const { return R; }
    consteval size_t GetCollumns() const { return C; }

private:
    ArrayType data;
};

template <typename T>
class VariableMatrix
{
public:
    VariableMatrix(size_t width, size_t height)
        : width(width)
        , height(height)
    {
        data.resize(width * height);
    }

    T* At(size_t x, size_t y)
    {
        if (x >= width)
            return nullptr;
        if (y >= height)
            return nullptr;

        return &data.at(y * width + x);
    }

    const T* At(size_t x, size_t y) const
    {
        if (x >= width)
            return nullptr;
        if (y >= height)
            return nullptr;

        return &data.at(y * width + x);
    }

    std::optional<VariableMatrix<T>> MatMul(const VariableMatrix<T>& other) const
    {
        if (width != other.height)
            return std::nullopt;
        size_t out_width = other.width;
        size_t out_height = height;

        VariableMatrix<T> out { out_width, out_height };

        for (size_t i = 0; i < out_height; i++)
        {
            for (size_t j = 0; j < out_width; j++)
            {
                for (size_t k = 0; k < width; k++)
                { // aColumn
                    (*out.At(j, i)) += (*At(k, i)) * (*other.At(j, k));
                }
            }
        }

        return out;
    }

    void Print()
    {
        for (size_t y = 0; y < height; y++)
        {
            for (size_t x = 0; x < width; x++)
            {
                std::cout << fmt::format("{:03} ", *At(x, y));
            }
            std::cout << "\n";
        }
    }

    size_t GetCollumns() const { return width; }
    size_t GetRows() const { return height; }

private:
    std::vector<T> data;
    size_t width, height;
};
