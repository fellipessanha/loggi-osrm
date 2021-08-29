#pragma once

#include <vector>

#include "loggi-include.hpp"
#include <OptFrame/Move.hpp>
#include <memory>
//

namespace loggibud {

using VRPMoveTemplate = optframe::FMove<std::pair<int, std::pair<int, int>>, ESolutionVRP>;

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
  std::function<optframe::Evaluation<double>(const std::vector<std::vector<int>>&)> fEvaluate1 =
    [this](const std::vector<std::vector<int>>& s) -> optframe::Evaluation<double> {
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
  std::function<std::vector<std::vector<int>>()>
    badGeneration = [this]() -> std::vector<std::vector<int>> {
    return firstBadSolution(this->allDeliveries, this->instance_.getCap());
  };
  //
  /////////////////////
  // Heuristic Moves //
  /////////////////////
  //


  optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE> optFC_evaluator; // CANNOT INITIALIZE HERE! ONLY IN CONSTRUCTOR!
  optframe::FConstructive<std::vector<std::vector<int>>> optFC_generator;           // CANNOT INITIALIZE HERE! ONLY IN CONSTRUCTOR!
  optframe::FConstructive<std::vector<std::vector<int>>> bad_optFC_generator;       // CANNOT INITIALIZE HERE! ONLY IN CONSTRUCTOR!

  FCoreLoggi(Instance& i)
    : instance_{ i }
    , capacity{ i.getCap() }
    , allDeliveries{ i.getAllDeliveries() }
    , optFC_evaluator{ fEvaluate1 }
    // , optFC_generator{ fGeneration1 }
    , bad_optFC_generator{ badGeneration }
  //, move_2Opt{ fRandom2Opt }
  {
    initial_routes = 0;
    for (loggibud::Delivery i : allDeliveries)
      initial_routes += i.size;
    initial_routes *= 1.3 / (capacity);
    std::cout << "\nfc constructed\n";
  }

}; // class
//
} // namespace loggibud