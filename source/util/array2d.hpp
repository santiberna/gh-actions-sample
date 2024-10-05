#pragma once
#include <cstddef>
#include <vector>

template <typename T>
class Array2D
{
public:
    Array2D(size_t width, size_t height);
    [[nodiscard]] size_t get_width() const { return m_width; }
    [[nodiscard]] size_t get_height() const { return m_height; }

    const T& operator()(size_t i, size_t j) const;
    T& operator()(size_t i, size_t j);

private:
    std::vector<T> m_data;
    size_t m_width {};
    size_t m_height {};
};

template <typename T>
inline Array2D<T>::Array2D(size_t width, size_t height)
    : m_data((width * height) + 1)
    , m_width(width)
    , m_height(height)
{
}

template <typename T>
inline const T& Array2D<T>::operator()(size_t i, size_t j) const
{
    return m_data[(j * m_width) + i];
}

template <typename T>
inline T& Array2D<T>::operator()(size_t i, size_t j)
{
    return m_data[(j * m_width) + i];
}
