
#include "loggi-include.hpp"

#include "distances-storage.hpp"

#include <cstdlib>

#include <assert.h>

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

  auto generated = loggibud::generatefromClusters(instance, instance.getCap());

  for (size_t route_idx = 0; route_idx < generated.size(); route_idx++) {
    assert(instance.get_cluster_of_route(route_idx) == instance.getClusters(generated[route_idx].back()));
  }

  for (int i = 0; i < instance.getClusterN(); i++) {
    auto& rte_by_cl = instance.get_routes_in_cluster(i);

    for (auto k : rte_by_cl) {
      std::cout << k << "\t";
    }
    std::cout << '\n';
  }
  return 0;
}