#pragma once

#include <vector>

#include "loggi-include.hpp"

namespace loggibud {

class FCoreLoggi
{
public:
  Instance instance_;
  const std::vector<Delivery>& allDeliveries;

  int initial_routes;
  int capacity;

  /*
   static optframe::Evaluation<double> evaluation(const std::vector<std::vector<int>>& s)
   {
      return optframe::Evaluation<double>{ evaluateInstance(s, allDeliveries) };
      // eval.outdated = false;
      // return eval;
   }
*/

  std::function<
    optframe::Evaluation<double>(const std::vector<std::vector<int>>& s)>
    fEvaluate1 = [this](const std::vector<std::vector<int>>& s) -> optframe::Evaluation<double> {
    return evaluateInstance(s, this->allDeliveries);
  };
  //{ evaluation };

  /*
   std::vector<std::vector<int>>
   generation()
   {
      return loggibud::pucaGenerator(allDeliveries, instance_.getCap(), initial_routes);
   }
*/
  std::function<
    std::vector<std::vector<int>>()>
    fGeneration1 = [this]() -> std::vector<std::vector<int>> {
    return loggibud::pucaGenerator(this->allDeliveries, this->instance_.getCap(), initial_routes);
  };

  optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE> optFEval; // CANNOT INITIALIZE HERE! ONLY IN CONSTRUCTOR!
  optframe::FConstructive<std::vector<std::vector<int>>> fC_generator;       // CANNOT INITIALIZE HERE! ONLY IN CONSTRUCTOR!

  FCoreLoggi(Instance i)
    : instance_{ i }
    , capacity{ i.getCap() }
    , allDeliveries{ i.getAllDeliveries() }
    , optFEval{ fEvaluate1 }
    , fC_generator{ fGeneration1 }
  {
    initial_routes = 0;
    for (loggibud::Delivery i : allDeliveries)
      initial_routes += i.size;
    initial_routes *= 1.3 / (capacity);
  };
}; // class
} // namespace loggibud