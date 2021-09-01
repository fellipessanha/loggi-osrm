#pragma once

#include <sstream>
//
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/cURLpp.hpp>

namespace loggibud {

std::vector<std::vector<double>>
evaluateDistsMatrix(const std::vector<int>& viableOptions, const std::vector<Delivery>& deliveries, bool origin = false)
{

  // Not sure if that's how i works, but cleaning up our http stuff here
  curlpp::Cleanup myCleanup;
  //
  // preparing the url that's gon be used
  std::stringstream ss;
  // ss.precision(4);

  ss << "localhost:5000/table/v1/driving/"
     // ss << "http://router.project-osrm.org/table/v1/driving/"
     << deliveries[viableOptions[0]].pt;
  //
  size_t usedOptions = viableOptions.size();
  for (size_t i = 1; i < usedOptions; i++) {
    ss << ';' << deliveries[viableOptions[i]].pt;
  }

  //
  // curlpp easymode fun
  curlpp::Easy myRequest;
  myRequest.setOpt<curlpp::options::Url>(ss.str());
  // after setting URL we can clear stringstream
  ss.str("");
  // making the request write straight to the ss stringstream avoids cluttering the cout
  myRequest.setOpt(curlpp::options::WriteStream(&ss));
  //
  try {
    myRequest.perform();
  } catch (curlpp::RuntimeError& e) {
    std::cout << e.what() << std::endl;
  }

  // setting up the json file
  json osrmOut;
  ss >> osrmOut;

  std::vector<std::vector<double>> distances = osrmOut["durations"];

  // store this in a variable
  // bugs if you do something like tensor.push_back(evaluateDistsMatrix(vec));
  return distances;
}

std::vector<double>
evaluateDistsfromPointString(const std::string& viableOptions, const std::string& source_coords, bool sources = true)
{

  // Not sure if that's how i works, but cleaning up our http stuff here
  curlpp::Cleanup myCleanup;
  //
  // preparing the url that's gon be used
  std::stringstream ss;
  // ss << "http://router.project-osrm.org/table/v1/driving/"
  ss << "localhost:5000/table/v1/driving/"
     << source_coords << ';' << viableOptions;
  //
  // Same options as in loggibud documentation
  if (sources) {
    ss << "?sources=0";
  } else {
    ss << "?destinations=0";
  }
  //
  // curlpp easymode fun
  curlpp::Easy myRequest;
  myRequest.setOpt<curlpp::options::Url>(ss.str());
  // after setting URL we can clear stringstream
  ss.str("");
  // making the request write straight to the ss stringstream avoids cluttering the cout
  myRequest.setOpt(curlpp::options::WriteStream(&ss));
  //
  try {
    myRequest.perform();
  } catch (curlpp::RuntimeError& e) {
    std::cout << e.what() << std::endl;
  }

  // setting up the json file
  // std::cout << "\n" << ss.str() << "\n";
  json osrmOut;
  ss >> osrmOut;

  auto aux = osrmOut["durations"];

  std::vector<double> outputs;
  if (sources) {
    size_t sz = aux[0].size();
    for (size_t i = 1; i < sz; i++) {
      outputs.push_back(aux[0][i]);
    }
  } else {
    size_t sz = aux.size();
    for (size_t i = 1; i < sz; i++) {
      outputs.push_back(aux[i][0]);
    }
  }

  return outputs;
}

double
evaluateRouteDistanceOSRM(const std::vector<int>& route, const std::vector<Delivery>& allDeliveries)
{

  // Not sure if that's how i works, but cleaning up our http stuff here
  curlpp::Cleanup myCleanup;
  //
  // preparing the url that's gon be used
  std::stringstream ss;
  ss.precision(4);
  ss << "localhost:5000/route/v1/driving/" << allDeliveries[route[0]].pt;
  // ss << "http://router.project-osrm.org/route/v1/driving/" << allDeliveries[route[0]].pt;
  // TODO: test to see how OSRM deals with it!
  for (int i = 1; i < route.size(); i++) {
    ss << ';' << allDeliveries[route[i]].pt;
  }
  // Same options as in loggibud documentation
  ss << ';' << allDeliveries[route[0]].pt;
  // ss << "?annotations=distance&continue_straight=false";
  //
  // curlpp easymode fun
  curlpp::Easy myRequest;
  myRequest.setOpt<curlpp::options::Url>(ss.str());
  // after setting URL we can clear stringstream
  ss.str("");
  // making the request write straight to the ss stringstream avoids cluttering the cout
  myRequest.setOpt(curlpp::options::WriteStream(&ss));
  //
  try {
    myRequest.perform();
  } catch (curlpp::RuntimeError& e) {
    std::cout << e.what() << std::endl;
  }

  // setting up the json file
  json osrmOut;
  ss >> osrmOut;
  //
  // returning sum of all distances
  return osrmOut["routes"][0]["distance"];
}

double
evaluateInstanceOSRM(const std::vector<std::vector<int>>& routes, const std::vector<Delivery>& allDeliveries)
{
  double instanceDistance = 0;
  for (auto i : routes) {
    double thisdist = evaluateRouteDistanceOSRM(i, allDeliveries);
    // std::cout << thisdist << '\t';
    instanceDistance += thisdist;
  }
  return instanceDistance;
}

double
evaluateRouteDistanceClusters(
  const std::vector<int>& route,
  Instance& instance,
  const size_t cluster_n)
{

  const auto& distMap = instance.getClusterMap(cluster_n);
  size_t route_sz = route.size();
  double routeDistance = instance.distFromOrigin(route.front()) + instance.distToOrigin(route.back());
  for (int stop = 1; stop < route_sz; stop++) {
    // assert(cluster_n == allDeliveries[route[stop]].cluster);
    routeDistance += distMap.at(route[stop - 1]).at(route[stop]);
  }
  return routeDistance;
}

double
evaluateInstanceClusters(
  const std::vector<std::vector<int>>& routes,
  Instance& instance)
{
  // std::cout << "Initializing evaluator" << std::endl;
  double instanceDistance = 0;
  for (auto route : routes) {
    const size_t route_cluster = instance.getClusters(route.back());
    // std::cout << "route cluster = " << route_cluster << '\n';

    // std::cout
    //   << "Evaluating specific route with cluster " << allDeliveries[route[0]].cluster
    //   << "\tdistMaps has size " << distMaps.size() << std::endl;
    for (auto stop : route)

      instanceDistance +=
        evaluateRouteDistanceClusters(
          route,
          instance,
          route_cluster);
  }
  return instanceDistance;
}

} // namespace

/*
std::vector<double>
evaluateDistsfromPoint(const std::vector<Delivery>& allDeliveries, const std::vector<int>& viableOptions, int source_index = 0, bool sources = true)
{

  // Not sure if that's how i works, but cleaning up our http stuff here
  curlpp::Cleanup myCleanup;
  //
  // preparing the url that's gon be used
  std::stringstream ss;
  // ss.precision(4);
  ss << "localhost:5000/table/v1/driving/" << allDeliveries[viableOptions[source_index]].pt;
  // ss << "http://router.project-osrm.org/table/v1/driving/" << allDeliveries[viableOptions[source_index]].pt;
  size_t usedOptions = viableOptions.size();
  for (size_t i = 0; i < usedOptions; i++) {
    ss << ';' << allDeliveries[viableOptions[i]].pt;
  }
  // Same options as in loggibud documentation
  if (sources){
    ss << "?sources=" << source_index;
  }
  else{
    ss << "?destinations=" << source_index;
  }
  //
  // curlpp easymode fun
  curlpp::Easy myRequest;
  myRequest.setOpt<curlpp::options::Url>(ss.str());
  // after setting URL we can clear stringstream
  ss.str("");
  // making the request write straight to the ss stringstream avoids cluttering the cout
  myRequest.setOpt(curlpp::options::WriteStream(&ss));
  //
  try {
    myRequest.perform();
  } catch (curlpp::RuntimeError& e) {
    std::cout << e.what() << std::endl;
  }

  // setting up the json file
  // std::cout << "\n" << ss.str() << "\n";
  json osrmOut;
  ss >> osrmOut;

  std::vector<double> outputs;
  for (size_t i = 0; i < usedOptions; i++) {
    outputs.push_back(osrmOut["durations"][0][i]);
  }
  return outputs;
}

*/