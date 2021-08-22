#include "loggi-include.hpp"

#include "FCore_functions.hpp"

int
main()
{
  // The case with CTAD - Class Template Argument Deduction (C++20)
  //std::vector ints{ 1, 2, 3, 4, 5, 6, 7 }; // deduced vector<int>

  // instance loading
  loggibud::Instance instance("data/cvrp-0-rj-0.json", "data/rj-0.json");
  auto& deliveriesList = instance.getAllDeliveries();
  loggibud::FCoreLoggi fC_loggi(instance);

  // auto sol = *fC_loggi.bad_optFC_generator.generateSolution(0);
  auto sol = *fC_loggi.optFC_generator.generateSolution(0);

  std::cout << '\n';

  // instancia NSSeq
  RandGen rg;

  NSSeq2Opt nsseq_2opt(instance, rg);


  // BasicSimulatedAnnealing(
    // sref<GeneralEvaluator<XES, XEv>> _evaluator
    // , sref<InitialSearch<XES, XEv>> _constructive
    // , sref<NS<XES, XEv, XSH>> _neighbors
    // , double _alpha
    // , int _SAmax
    // , double _Ti
    // , sref<RandGen> _rg = new RandGen)

  optframe::BasicSimulatedAnnealing()

  ev = 

  return 0;
}