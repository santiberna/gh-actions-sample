#pragma once
#include <vector>
#include <unordered_map>
#include <optional>
#include <iostream>
#include <cstring>

template <typename T>
class Grid
{
public:
    Grid(size_t width, size_t height)
        : width(width)
        , height(height)
    {
        data.resize(width * height);
    }

    Grid(T* start, size_t width, size_t height)
        : width(width)
        , height(height)
    {
        data.resize(width * height);
        std::memcpy(data.data(), start, width * height * sizeof(T));
    }

    std::optional<T> At(size_t x, size_t y) const
    {
        if (x >= width)
            return {};
        if (y >= height)
            return {};

        return data[y * width + x];
    }

    void Set(size_t x, size_t y, T val)
    {
        if (x >= width)
            throw;
        if (y >= height)
            throw;

        data[y * width + x] = val;
    }

    void Print() const
    {
        for (size_t j = 0; j < height; j++)
        {
            for (size_t i = 0; i < width; i++)
            {
                std::cout << data[j * width + i] << " ";
            }
            std::cout << "\n";
        }
    }

private:
    size_t width, height;
    std::vector<T> data;
};

class AdjacencyMatrix
{
public:
    AdjacencyMatrix(size_t size);

    void AddNode();

    void AddUndirectedEdge(size_t start, size_t end, float weight);
    void AddDirectedEdge(size_t start, size_t end, float weight);

    float* At(size_t x, size_t y);

    void Print();
    size_t GetSize() const { return width; }

private:
    size_t width;
    std::vector<float> matrix;
};