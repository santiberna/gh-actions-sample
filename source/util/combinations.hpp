#pragma once
#include "array2d.hpp"
#include <cstddef>
#include <vector>

size_t factorial(size_t n);

size_t number_of_permutations(size_t set_size, size_t comb_size);
size_t number_of_combinations(size_t set_size, size_t comb_size);

Array2D<int> find_combinations(const std::vector<int>& set, size_t k);