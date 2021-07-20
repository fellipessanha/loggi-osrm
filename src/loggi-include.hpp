// Include all headers:
#pragma once
//
#include <OptFCore/FCore.hpp>
#include <OptFrame/Core.hpp>
//
using ESolutionVRP = std::pair<
  std::vector<std::vector<int>>,
  optframe::Evaluation<double>>;
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
#include "loggi-context-loader.hpp"
//
#include "loggi-evaluator.cpp"
#include "loggi-solution-generators.cpp"
//
//#include "FCore_functions.hpp"