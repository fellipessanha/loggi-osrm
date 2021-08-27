
#include "loggi-include.hpp"

#include "distances-storage.hpp"

// #include "FCore_functions.hpp"

class MyEvaluator final
  : public optframe::Evaluator<ESolutionVRP::first_type, ESolutionVRP::second_type, ESolutionVRP>
{
  
public:
  const loggibud::Instance& instance;
  const std::vector<loggibud::Delivery> deliveriesList;
  //
  // can't be const, should ask Igor why
  optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE>& ev;

  //, std::function<optframe::Evaluation<double>(std::vector<std::vector<int>>)> _feval)
  MyEvaluator(loggibud::Instance _instance, optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE>& _ev)
    : instance{ _instance }
    , deliveriesList{ instance.getAllDeliveries() }
    , ev { _ev}
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
main()
{
  // loading instance data
  loggibud::Instance instance("data/cvrp-0-rj-0.json", "data/rj-0.json", "clusterings/kmeans-clusteringLabels.json");
  auto& deliveriesList = instance.getAllDeliveries();

  /////////////////////////// 
  // getting relevant info //
  /////////////////////////// 
  //
  const int cap = instance.getCap();
  const int min_cars = deliveriesList.size() / cap;

  //////////////////////////////
  // setting up the evaluator //
  //////////////////////////////
  //
  std::function<optframe::Evaluation<double>(std::vector<std::vector<int>>)>
    evaluation_function = [&](std::vector<std::vector<int>> sol) -> optframe::Evaluation<double> {
    double score = loggibud::evaluateInstance(sol, deliveriesList);
    return optframe::Evaluation<double>{ score };
  };
  //
  optframe::FEvaluator<ESolutionVRP, optframe::MinOrMax::MINIMIZE> opt_evaluator{evaluation_function};
  //
  MyEvaluator myEval(instance, opt_evaluator);


  ///////////////////////////////////////////////
  // setting up the initial solution generator //
  ///////////////////////////////////////////////
  // this depends on the Evaluator!
  //
  std::function<std::vector<std::vector<int>>()>
    badGeneration = [&]() -> std::vector<std::vector<int>> {
    return firstBadSolution(deliveriesList, instance.getCap());
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

  vsref<optframe::NS<ESolutionVRP>> neighbors;
  neighbors.push_back(nsSeq2Opt);

  auto x = loggibud::makeMatrixDistances(instance);

  for (auto i= x[10].begin(); i != x[10].end(); ++i)
    std::cout << i->first << '\t';

  exit(1);
  /////////////////////////////////////////
  // Setting up the Simmulated Annealing //
  /////////////////////////////////////////
  //
  optframe::BasicSimulatedAnnealing<ESolutionVRP> sa(
    myEval,             // sref<GeneralEvaluator<XES, XEv>> _evaluator,
    badInit,            // sref<InitialSearch<XES, XEv>> _constructive,
    neighbors,          // sref<NS<XES, XEv, XSH>> _neighbors,
    0.98, 500, 1000,    // double _alpha, int _SAmax, double _Ti,
    rg                  // sref<RandGen> _rg = new RandGen
  );

  sa.setSilentR();

  auto status = sa.search(30);

  return 0;
} 