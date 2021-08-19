// Include all headers:
#pragma once
//
#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
//
#include <OptFrame/Heuristics/LocalSearches/BestImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/FirstImprovement.hpp>
#include <OptFrame/Heuristics/LocalSearches/VariableNeighborhoodDescent.hpp>
//
#include <OptFrame/Heuristics/ILS/IteratedLocalSearchLevels.hpp>

//
using ESolutionVRP = std::pair<
  std::vector<std::vector<int>>,
  optframe::Evaluation<double> >;
//
using uptrMoveVPR = std::unique_ptr<optframe::Move<ESolutionVRP>>
//
#include <fstream>
#include <set>
#include <sstream>
//
#include "../nlohmann/json.hpp"
using json = nlohmann::json;
//
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>
//
#include "my_utils.hpp"
#include "loggi-context-loader.hpp"
//
#include "loggi-evaluator.cpp"
#include "loggi-solution-generators.cpp"
#include "heuristic_moves.hpp"
#include "NSSeq2Opt.hpp"
//
//#include "FCore_functions.hpp"