#include "combinations.hpp"
#include <numeric>
#include <algorithm>

Array2D<int> FindCombinations(const std::vector<int>& set, size_t k)
{
    Array2D<int> out { k, NumberOfCombinations(set.size(), k) };

    std::vector<bool> v(set.size());
    std::fill(v.begin() + k, v.end(), true);

    size_t row = 0;

    do
    {
        size_t column = 0;
        for (size_t i = 0; i < set.size(); ++i)
        {
            if (!v[i])
            {
                out(column++, row) = set[i];
            }
        }
        row++;

    } while (std::next_permutation(v.begin(), v.end()));

    return out;
}

size_t Factorial(size_t n)
{
    size_t out = n;
    while (--n)
    {
        out *= n;
    }
    return out;
}

size_t NumberOfPermutations(size_t set_size, size_t comb_size)
{
    if (set_size < comb_size)
        return 0;

    size_t t = set_size - comb_size;
    size_t q = set_size;

    while (--set_size > t)
    {
        q *= set_size;
    }

    return q;
}

size_t NumberOfCombinations(size_t set_size, size_t comb_size)
{
    if (set_size < comb_size)
        return 0;
    return NumberOfPermutations(set_size, comb_size) / Factorial(comb_size);
}
