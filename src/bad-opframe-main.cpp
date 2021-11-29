
#include "loggi-include.hpp"

#include "distances-storage.hpp"

#include <cstdlib>

#include <OptFrame/Heuristics/LocalSearches/HillClimbing.hpp>
#include <OptFrame/Heuristics/LocalSearches/MultiImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/MultiRandomSelection.hpp>
#include <OptFrame/Heuristics/LocalSearches/RandomDescentMethod.hpp>
#include <OptFrame/Util/CheckCommand.hpp>
#include <OptFrame/printable/printable.hpp>

// #include "FCore_functions.hpp"

std::vector<string> deliveries_path{
  "dataset/delivery-instances-1.0/train/rj/rj-0.json",
  "dataset/delivery-instances-1.0/train/rj/rj-1.json",
  "dataset/delivery-instances-1.0/train/rj/rj-2.json",
  "dataset/delivery-instances-1.0/train/rj/rj-3.json",
  "dataset/delivery-instances-1.0/train/rj/rj-4.json",
  "dataset/delivery-instances-1.0/train/rj/rj-5.json",
  "dataset/delivery-instances-1.0/train/rj/rj-6.json",
  "dataset/delivery-instances-1.0/train/rj/rj-7.json",
  "dataset/delivery-instances-1.0/train/rj/rj-8.json",
  "dataset/delivery-instances-1.0/train/rj/rj-9.json",
  "dataset/delivery-instances-1.0/train/rj/rj-10.json",
};

std::vector<string> instances_path{
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-0.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-1.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-2.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-3.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-4.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-5.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-6.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-7.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-8.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-9.json",
  "dataset/cvrp-instances-1.0/train/rj-0/cvrp-0-rj-10.json",
};

class MyEvaluator final : public optframe::Evaluator<ESolutionVRP::first_type, ESolutionVRP::second_type, ESolutionVRP>
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
  std::stringstream outBuffer;
  outBuffer << "instance, rand_score, hillClimb_score, RDM_score, SA_score\n";
  for (int instance_index = 0; instance_index < deliveries_path.size(); instance_index++) {
    for (int repetitions = 20; repetitions < 30; repetitions++) {
      outBuffer << instance_index << "_" << repetitions << ", ";

      bool check_verbose = false;
      optframe::CheckCommand<ESolutionVRP> checkModule(check_verbose);

      // loading instance data
      loggibud::Instance instance(
        instances_path[instance_index],
        deliveries_path[instance_index],
        "clusterings/kmeans-clusteringLabels.json");
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
      sref<optframe::InitialSearch<ESolutionVRP>> initBadref{ badInit };

      sref<optframe::RandGen> rg{ new optframe::RandGen };
      //
      sref<optframe::NS<ESolutionVRP>> nsSeq2Opt{ new loggibud::NSSeq2Opt(instance, rg) };

      vsref<optframe::NS<ESolutionVRP>> neighbors;
      neighbors.push_back(nsSeq2Opt);

      // checkModule.add(badInit);
      // checkModule.add(myEval);
      // checkModule.add(nsSeq2Opt);

      // auto allData = checkModule.run(5, 8);
      // // general_param, qtas voltas vc quer dar na vizinhança
      // exit(1);

      /////////////////////////////////////////
      // Setting up the heuristics //
      /////////////////////////////////////////
      //
      optframe::StopCriteria<optframe::Evaluation<double>> sc{ 0 };

      // Multi improvement for support:
      sref<optframe::LocalSearch<ESolutionVRP>> mirs_tsp{ new optframe::MultiRandomSelection<ESolutionVRP>(myEval, nsSeq2Opt, deliveriesList.size(), { 0 }) };

      // Hill Climb
      sref<optframe::LocalSearch<ESolutionVRP>> hc_mirs_tsp{ new optframe::HillClimbing<ESolutionVRP>{ myEval, mirs_tsp } };

      ESolutionVRP init_hill = *initBadref->initialSearch(sc.start()).first;
      std::cout << "initial SECOND: " << init_hill.second << std::endl;
      outBuffer << init_hill.second << ", ";

      auto status_hill = hc_mirs_tsp->searchFrom(init_hill, sc.start());

      // std::cout << "HillClimb FIRST: " << init_hill.first << std::endl;
      std::cout << "HillClimb SECOND: " << init_hill.second << std::endl;
      outBuffer << init_hill.second << ", ";

      // Random Descent

      sref<optframe::LocalSearch<ESolutionVRP>>
        rdm_tsp{ new optframe::RandomDescentMethod<ESolutionVRP>{ myEval, nsSeq2Opt, deliveriesList.size() } };

      ESolutionVRP init_descent = *initBadref->initialSearch(sc.start()).first;

      auto status_descent = rdm_tsp->searchFrom(init_descent, sc.start());

      // std::cout << "st: " << st3 << " time:" << sc.getTime() << std::endl;
      // std::cout << "PRINT FIRST: " << init_descent.first << std::endl;
      std::cout << "random descent SECOND: " << init_descent.second << std::endl;
      outBuffer << init_descent.second;

      // SA
      double tempInit = optframe::BasicSimulatedAnnealing<ESolutionVRP>::estimateInitialTemperature(
        myEval, badInit, neighbors, 1.1, 0.7, 500, 1, rg);

      optframe::BasicSimulatedAnnealing<ESolutionVRP>
        sa(
          myEval,    // sref<GeneralEvaluator<XES, XEv>> _evaluator,
          badInit,   // sref<InitialSearch<XES, XEv>> _constructive,
          nsSeq2Opt, // sref<NS<XES, XEv, XSH>> _neighbors,
          0.98,
          100,
          tempInit, // double _alpha, int _SAmax, double _Ti,
          rg        // sref<RandGen> _rg = new RandGen
        );

      sa.setSilentR();

      auto status = sa.search(60);
      auto sa_best = *status.best;
      sa_best.second.print();

      outBuffer << sa_best.second.evaluation() << '\n';
    }
  }
  std::ofstream logswriter("logs/real_late_log.csv");
  logswriter << outBuffer.str();
  return 0;
}