#pragma once

#include <utility>
#include <random>

std::pair<int, int> twoRandNoDepot(int max)
{
  // i HAS to be smaller than j
  int i = (rand() % (max - 2)) + 1;
  // 1 <= j <= pTSP.n - i; j is random
  int j = i + (rand() % (max - i - 1)) + 1; // TODO: check!!
  return std::make_pair(i, j);
}