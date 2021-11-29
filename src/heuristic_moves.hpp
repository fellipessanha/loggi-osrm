#pragma once

#include "MoveData.hpp"

//#include "loggi-include.hpp"
#include "my_utils.hpp"

namespace loggibud {
//
// intrarroute
const MoveData&
opt02(const MoveData& moveData, ESolutionVRP& candidate)
{
  assert(moveData.route1 < candidate.first.size());

  std::vector<int>& route = candidate.first.at(moveData.route1);
  int n = route.size();

  // std::cout << "MoveData: (" << moveData.route1 << ", {" << moveData.limitsL.first << ", " << moveData.limitsL.second << "});"
  //           << "\t size = " << n << std::endl;

  auto optLimits = moveData.limitsL;

  assert(optLimits.first <= optLimits.second);
  assert(optLimits.first < n);
  assert(optLimits.second < n);

  // std::reverse(route.begin() + optLimits.first, route.begin() + optLimits.second);

  n = abs(optLimits.second - optLimits.first) / 2;

  for (auto i = 0; i < n; i++) {
    int j = route.at(optLimits.first + i);
    route.at(optLimits.first + i) = route.at(optLimits.second - i);
    route.at(optLimits.second - i) = j;
  }

  return moveData;
}

// interroute
void
swap01(std::vector<std::vector<int>>& candidate)
{
  int n = candidate.size();
  auto routes = twoRandNoDepot(n);

  const int i = (rand() % (candidate[routes.first].size() - 1)) + 1;
  const int j = (rand() % (candidate[routes.second].size() - 1)) + 1;

  n = candidate[routes.first][i];

  candidate[routes.first][i] = candidate[routes.second][j];
  candidate[routes.second][j] = candidate[routes.first][i];
}

void
shift20(std::vector<std::vector<int>>& candidate)
{
  int n = candidate.size();
  auto routes = twoRandNoDepot(n);

  // shifts 2 adjacent stops from aRoute to bRoute
  auto& aRoute = candidate[routes.first];
  auto& bRoute = candidate[routes.second];

  int pos = (rand() % (bRoute.size() - 2)) + 1;

  int stops[2] = { bRoute[pos], bRoute[pos + 1] };

  // inserting in random position for now
  //  maybe think about using OSRM to figure a good insert position later
  n = (rand() % aRoute.size() - 1) + 1;

  aRoute.insert(aRoute.begin() + n, stops, stops + 2);
  bRoute.erase(bRoute.begin() + pos, bRoute.begin() + (pos + 2));
}

void
cross(std::vector<std::vector<int>>& candidate)
{

  int n = candidate.size();
  auto routes = twoRandNoDepot(n);

  auto& aRoute = candidate[routes.first];
  auto& bRoute = candidate[routes.second];

  int i = rand() % (aRoute.size() - 1) + 1;
  int j = rand() % (bRoute.size() - 1) + 1;

  std::vector<int> aux(aRoute.begin() + i, aRoute.end());

  aRoute.erase(aRoute.begin() + i, aRoute.end());
  aRoute.insert(aRoute.end(), bRoute.begin() + j, bRoute.end());

  bRoute.erase(bRoute.begin() + j, bRoute.end());
  bRoute.insert(bRoute.end(), aux.begin(), aux.end());
}

} // namespace