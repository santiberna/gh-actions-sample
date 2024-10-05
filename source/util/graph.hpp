#pragma once
#include <vector>
#include <optional>
#include <iostream>
#include <cstring>

template <typename T>
class Grid
{
public:
    Grid(size_t width, size_t height)
        : m_width(width)
        , m_height(height)
    {
        m_data.resize(width * height);
    }

    Grid(T* start, size_t width, size_t height)
        : m_width(width)
        , m_height(height)
    {
        m_data.resize(width * height);
        std::memcpy(m_data.data(), start, width * height * sizeof(T));
    }

    std::optional<T> at(size_t x, size_t y) const
    {
        if (x >= m_width) {
            return {};
}
        if (y >= m_height) {
            return {};
}

        return m_data[(y * m_width) + x];
    }

    void set(size_t x, size_t y, T val)
    {
        if (x >= m_width) {
            throw;
}
        if (y >= m_height) {
            throw;
}

        m_data[(y * m_width) + x] = val;
    }

    void print() const
    {
        for (size_t j = 0; j < m_height; j++)
        {
            for (size_t i = 0; i < m_width; i++)
            {
                std::cout << m_data[(j * m_width) + i] << " ";
            }
            std::cout << "\n";
        }
    }

private:
    size_t m_width, m_height;
    std::vector<T> m_data;
};

class AdjacencyMatrix
{
public:
    AdjacencyMatrix(size_t size);

    void add_node();

    void add_undirected_edge(size_t start, size_t end, float weight);
    void add_directed_edge(size_t start, size_t end, float weight);

    float* at(size_t x, size_t y);

    void print();
    [[nodiscard]] size_t get_size() const { return m_width; }

private:
    size_t m_width;
    std::vector<float> m_matrix;
};