#pragma once

#include "loggi-include.hpp"

namespace loggibud
{
class FCoreLoggi{
public:

  Instance instance_;
  const std::vector<Delivery> &allDeliveries;
  int initial_routes;
  int capacity;
  

  static optframe::Evaluation<double> evaluation(const std::vector<std::vector<int>> &s){
    return optframe::Evaluation<double> eval{evaluateInstance(s, allDeliveries)};
    // eval.outdated = false;
    // return eval;
  }
  
  std::vector<std::vector<int>> generation(){
    return loggibud::pucaGenerator(allDeliveries, instance.getCap, initial_routes);
  }

  optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE> optFEval
  (
    evaluation  // now static
  );

  optframe::FConstructive<std::vector<std::vector<int>>> fC_generator{
    generation
  };
  


  FCoreLoggi(Instance i):
  instance_(i), capacity(i.getCap()), allDeliveries(i.getAllDeliveries()){
    initial_routes = 0;
    for (loggibud::Delivery i : allDeliveries)
      initial_routes += i.size;
    initial_routes *= 1.3/(capacity);
  };
};  // class
}   // namespace