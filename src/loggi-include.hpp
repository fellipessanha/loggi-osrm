// Include all headers:
#pragma once
//
#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
//
using ESolutionVRP = std::pair<
  std::vector<std::vector<int>>, optframe::Evaluation<double> 
  >;
//
#include <fstream>
#include <sstream>
#include <set>
//
#include "../nlohmann/json.hpp"
using json = nlohmann::json;
//
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
//
#include "loggi-context-loader.hpp"
//
#include "loggi-evaluator.cpp"
#include "loggi-solution-generators.cpp"
//
#include "FCore_functions.hpp"