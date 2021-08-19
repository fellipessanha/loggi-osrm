#pragma once

namespace loggibud {

struct MoveData
{
  // route 1 has limits1. It is used in intrarroute moves
  // this means a Swap in route1 = 5 and limits1= {3, 10} would
  // swap stops 3 and 10 in route 5;
  int route1;
  std::pair<int, int> limits1;
  // route2 is used for interroute moves, like an interroute swap
  int route2 = 0;
  std::pair<int, int> limits2 = std::make_pair(0, 0);

  // Constructor for simple intrarroute moves
  MoveData(int r1, std::pair<int, int> lim)
    : route1{ r1 }
    , limits1{ lim }
  {}

  // Constructor for simple interroute moves
  MoveData(int r1, int r2, const std::pair<int, int>& lim)
    : route1{ r1 }
    , route2{ r2 }
    , limits1{ lim }
  {}

  MoveData(int r1, const std::pair<int, int>& lim1, int r2, const std::pair<int, int>& lim2)
    : route1{ r1 }
    , limits1{ lim1 }
    , route2{ r2 }
    , limits2{ lim2 }
  {}

  MoveData(MoveData&& mv)
    : route1{ mv.route1 }
    , limits1{ mv.limits1 }
    , route2{ mv.route2 }
    , limits2{ mv.limits2 }
  {}

  MoveData(MoveData& mv)
    : route1{ mv.route1 }
    , limits1{ mv.limits1 }
    , route2{ mv.route2 }
    , limits2{ mv.limits2 }
  {}

  MoveData(MoveData* mv)
    : route1{ mv->route1 }
    , limits1{ mv->limits1 }
    , route2{ mv->route2 }
    , limits2{ mv->limits2 }
  {}

  MoveData()
  {}
};

} // namespace loggibud