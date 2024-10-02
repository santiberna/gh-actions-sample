#pragma once
#include "array2d.hpp"

size_t Factorial(size_t n);

size_t NumberOfPermutations(size_t set_size, size_t comb_size);
size_t NumberOfCombinations(size_t set_size, size_t comb_size);

Array2D<int> FindCombinations(const std::vector<int>& set, size_t k);