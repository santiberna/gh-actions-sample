#include "graph.hpp"

#include <iostream>
#include <format>

AdjacencyMatrix::AdjacencyMatrix(size_t size)
    : width(size)
{
    matrix.resize(width * width, 0.0f);
}

void AdjacencyMatrix::AddNode()
{
    std::vector<float> new_matrix;
    new_matrix.resize((width + 1) * (width + 1));

    for (size_t y = 0; y < width; y++)
        for (size_t x = 0; x < width; x++)
            new_matrix[x + y * (width + 1)] = *At(x, y);

    matrix = new_matrix;
    width += 1;
}

void AdjacencyMatrix::AddUndirectedEdge(size_t start, size_t end, float weight)
{
    auto* start_to_end = At(start, end);
    auto* end_to_start = At(end, start);

    if (start_to_end && end_to_start)
    {
        (*start_to_end) = weight;
        (*end_to_start) = weight;
    }
}

void AdjacencyMatrix::AddDirectedEdge(size_t start, size_t end, float weight)
{
    auto* start_to_end = At(start, end);

    if (start_to_end)
    {
        (*start_to_end) = weight;
    }
}

float* AdjacencyMatrix::At(size_t x, size_t y)
{
    if (x >= width)
        return nullptr;
    if (y >= width)
        return nullptr;

    return &matrix.at(y * width + x);
}

void AdjacencyMatrix::Print()
{
    for (int y = 0; y < width; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float val = *At(x, y);
            std::cout << std::format("{:03} ", val);
        }
        std::cout << "\n";
    }
}
