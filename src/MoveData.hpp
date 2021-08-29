#pragma once

namespace loggibud {

struct MoveData
{
  // route 1 has limitsL. It is used in intrarroute moves
  // this means a Swap in route1 = 5 and limitsL= {3, 10} would
  // swap stops 3 and 10 in route 5;
  int route1;
  std::pair<int, int> limitsL;
  // route2 is used for interroute moves, like an interroute swap
  int route2 = 0;
  std::pair<int, int> limitsR = std::make_pair(0, 0);

  // Constructor for simple intrarroute moves
  MoveData(int r1, std::pair<int, int> lim)
    : route1(r1)
    , limitsL(lim)
  {
    std::cout  << "\ninside constructor:\t"
    << route1 << '\t' << limitsL.first << ", " << limitsL.second << std::endl;
  }

  // Constructor for simple interroute moves
  MoveData(int r1, int r2, const std::pair<int, int>& lim)
    : route1{ r1 }
    , route2{ r2 }
    , limitsL{ lim }
  {}

  MoveData(int r1, const std::pair<int, int>& lim1, int r2, const std::pair<int, int>& lim2)
    : route1{ r1 }
    , limitsL{ lim1 }
    , route2{ r2 }
    , limitsR{ lim2 }
  {}

  MoveData(const MoveData& mv)
  {
    route1 = mv.route1;
    limitsL = mv.limitsL;
    route2 = mv.route2;
    limitsR = mv.limitsR;
    std::cout << "using constructor by MoveData:\t" << route1 << '\t' << limitsL.first << '\t' << limitsL.second << std::endl;
  }

  MoveData(MoveData&& mv)
    : route1{ mv.route1 }
    , limitsL{ mv.limitsL }
    , route2{ mv.route2 }
    , limitsR{ mv.limitsR }
  {
    std::cout << "using constructor by MoveData&&:\t" << route1 << '\t' << limitsL.first << '\t' << limitsL.second << std::endl;
  }

  MoveData(MoveData& mv)
    : route1{ mv.route1 }
    , limitsL{ mv.limitsL }
    , route2{ mv.route2 }
    , limitsR{ mv.limitsR }
  {
    std::cout << "using constructor by MoveData&:\t" << route1 << '\t' << limitsL.first << '\t' << limitsL.second << std::endl;
  }

  MoveData(MoveData* mv)
    : route1{ mv->route1 }
    , limitsL{ mv->limitsL }
    , route2{ mv->route2 }
    , limitsR{ mv->limitsR }
  {
    std::cout << "using constructor by MoveData*:\t" << route1 << '\t' << limitsL.first << '\t' << limitsL.second << std::endl;
  }

  MoveData()
  {}
};

} // namespace loggibud