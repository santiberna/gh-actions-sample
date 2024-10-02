#pragma once
#include <vector>

template <typename T>
class Array2D
{
public:
    Array2D(size_t width, size_t height);
    size_t GetWidth() const { return width; }
    size_t GetHeight() const { return height; }

    const T& operator()(size_t i, size_t j) const;
    T& operator()(size_t i, size_t j);

private:
    std::vector<T> data;
    size_t width {};
    size_t height {};
};

template <typename T>
inline Array2D<T>::Array2D(size_t width, size_t height)
    : data(width * height + 1)
    , width(width)
    , height(height)
{
}

template <typename T>
inline const T& Array2D<T>::operator()(size_t i, size_t j) const
{
    return data[j * width + i];
}

template <typename T>
inline T& Array2D<T>::operator()(size_t i, size_t j)
{
    return data[j * width + i];
}
