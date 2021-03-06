
#include "loggi-include.hpp"

#include "distances-storage.hpp"

#include <cstdlib>

// #include "FCore_functions.hpp"

class MyEvaluator final
  : public optframe::Evaluator<ESolutionVRP::first_type, ESolutionVRP::second_type, ESolutionVRP>
{

public:
  const loggibud::Instance& instance;
  const std::vector<loggibud::Delivery> deliveriesList;
  //
  optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE>& ev;

  //, std::function<optframe::Evaluation<double>(std::vector<std::vector<int>>)> _feval)
  MyEvaluator(loggibud::Instance _instance, optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE>& _ev)
    : instance{ _instance }
    , deliveriesList{ instance.getAllDeliveries() }
    , ev{ _ev }
  {}

  virtual optframe::Evaluation<double> evaluate(const std::vector<std::vector<int>>& sol) override
  {
    // invoke heuristic evaluator
    optframe::Evaluation<double> e_h = ev.evaluate(sol);
    return e_h.evaluation();
  }

  virtual bool isMinimization() const { return true; }
};

int
main(int argc, const char** argv)
{

  int time;
  if (argc > 1)
    time = std::atoi(argv[1]);
  else
    time = 60;

  // loading instance data
  loggibud::Instance instance("data/cvrp-0-rj-0.json", "data/rj-0.json", "clusterings/kmeans-clusteringLabels.json");
  ///////////////////////////
  // getting relevant info //
  ///////////////////////////
  //
  const auto& deliveriesList = instance.getAllDeliveries();
  // const auto clustersMap = loggibud::makeMatrixDistances(instance);
  const int cap = instance.getCap();
  const int min_cars = deliveriesList.size() / cap;

  //////////////////////////////
  // setting up the evaluator //
  //////////////////////////////
  //
  std::function<optframe::Evaluation<double>(std::vector<std::vector<int>>)>
    evaluation_function = [&](std::vector<std::vector<int>> sol) -> optframe::Evaluation<double> {
    double score = loggibud::evaluateInstanceClusters(sol, instance);
    return optframe::Evaluation<double>{ score };
  };
  //
  optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE> opt_evaluator{ evaluation_function };
  //
  MyEvaluator myEval(instance, opt_evaluator);

  ///////////////////////////////////////////////
  // setting up the initial solution generator //
  ///////////////////////////////////////////////
  // this depends on the Evaluator!
  //
  std::function<std::vector<std::vector<int>>()>
    badGeneration = [&]() -> std::vector<std::vector<int>> {
    return loggibud::generatefromClusters(instance, instance.getCap());
  };
  //
  optframe::FConstructive<std::vector<std::vector<int>>> badGen{
    badGeneration
  };
  //
  optframe::BasicInitialSearch<ESolutionVRP> badInit(badGen, opt_evaluator);

  sref<optframe::RandGen> rg{ new optframe::RandGen };
  //
  sref<loggibud::NSSeq2Opt> nsSeq2Opt{ new loggibud::NSSeq2Opt(instance, rg) };
  // sref<loggibud::NSSeqSwap10> nsSeqSwap10{ new loggibud::NSSeqSwap10(instance, rg) };

  vsref<optframe::NS<ESolutionVRP>> neighbors;
  neighbors.push_back(nsSeq2Opt);
  // neighbors.push_back(nsSeqSwap10);

  /////////////////////////////////////////
  // Setting up the Simmulated Annealing //
  /////////////////////////////////////////
  //
  optframe::BasicSimulatedAnnealing<ESolutionVRP> sa(
    myEval,    // sref<GeneralEvaluator<XES, XEv>> _evaluator,
    badInit,   // sref<InitialSearch<XES, XEv>> _constructive,
    neighbors, // sref<NS<XES, XEv, XSH>> _neighbors,
    0.98,
    500,
    1000, // double _alpha, int _SAmax, double _Ti,
    rg    // sref<RandGen> _rg = new RandGen
  );

  sa.setSilentR();

  auto status = sa.search(time);

  return 0;
}