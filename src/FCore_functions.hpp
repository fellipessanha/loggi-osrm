#pragma once

#include <vector>

#include "loggi-include.hpp"
#include <OptFrame/Move.hpp>
#include <memory>
//

namespace loggibud {

using VRPMoveTemplate = optframe::FMove< std::pair<int, std::pair<int, int>>, ESolutionVRP>;

class FCoreLoggi
{
  Instance instance_;
  const std::vector<Delivery>& allDeliveries;
  int initial_routes;
  int capacity;
  //
  //////////////////
  //  Evaluation  //
  //////////////////
  //
  optframe::Evaluation<double> evaluation(const std::vector<std::vector<int>>& s)
  {
    return optframe::Evaluation<double>(evaluateInstance(s, allDeliveries));
  }
  //
  std::function<
    optframe::Evaluation<double>(const std::vector<std::vector<int>>&)>
    fEvaluate1 = [this](const std::vector<std::vector<int>>& s) -> optframe::Evaluation<double> {
    return evaluateInstance(s, this->allDeliveries);
  };
  //
  //////////////////
  //  Generation  //
  //////////////////
  //
  std::vector<std::vector<int>>
  generation()
  {
    return pucaGenerator(allDeliveries, instance_.getCap(), initial_routes);
  }
  //
  std::function<
    std::vector<std::vector<int>>()>
    fGeneration1 = [this]() -> std::vector<std::vector<int>> {
    return pucaGenerator(this->allDeliveries, this->instance_.getCap(), initial_routes);
  };
  //
  std::function<
    std::vector<std::vector<int>>()>
    badGeneration = [this]() -> std::vector<std::vector<int>> {
    return firstBadSolution(this->allDeliveries, this->instance_.getCap());
  };
  //
  /////////////////////
  // Heuristic Moves //
  /////////////////////
  //
  uptrMoveVPR >
  random2Opt(const ESolutionVRP& candidate)
  {
    const std::vector<std::vector<int>>& rt = candidate.first;
    int n = rt.size();
    int route_n = 1 + (rand() % (n - 1));
    //
    auto optLimits = twoRandNoDepot(rt[route_n].size());
    //
    std::pair<int, std::pair<int, int>> mvData;
    mvData.first = route_n;
    mvData.second = optLimits;

    return uptrMoveVPR >{
      new VRPMoveTemplate(std::make_pair(route_n, optLimits), opt02)};
  }
  //
  std::function<
    uptrMoveVPR >(const ESolutionVRP&) >
    fRandom2Opt = this->random2Opt;

//
public:
  optframe::FNS<ESolutionVRP> move_2Opt;
  optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE> optFC_evaluator; // CANNOT INITIALIZE HERE! ONLY IN CONSTRUCTOR!
  optframe::FConstructive<std::vector<std::vector<int>>> optFC_generator;           // CANNOT INITIALIZE HERE! ONLY IN CONSTRUCTOR!
  optframe::FConstructive<std::vector<std::vector<int>>> bad_optFC_generator;       // CANNOT INITIALIZE HERE! ONLY IN CONSTRUCTOR!

  FCoreLoggi(Instance& i)
    : instance_ { i }
    , capacity { i.getCap() }
    , allDeliveries { i.getAllDeliveries() }
    , optFC_evaluator { evaluation }
    , optFC_generator { generation }
    , bad_optFC_generator { badGeneration }
    , move_2Opt { fRandom2Opt }
  {
    initial_routes = 0;
    for (loggibud::Delivery i : allDeliveries)
      initial_routes += i.size;
    initial_routes *= 1.3 / (capacity);
    std::cout << "\nfc constructed\n";
  };
}; // class
} // namespace loggibud