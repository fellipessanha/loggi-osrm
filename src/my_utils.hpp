#pragma once

#include <utility>
#include <random>

std::pair<size_t, size_t> twoRandNoDepot(size_t max)
{
  // i HAS to be smaller than j
  // std::cout << (max - 2) << '\t';
  size_t i = (rand() % (max - 2))
  // 1 <= j <= pTSP.n - i; j is random
  // std::cout << i << '\n' << (max-i-1) << '\t';
  size_t j = i + (rand() % (max - i - 1)) + 1;
  return std::make_pair(i, j);
}