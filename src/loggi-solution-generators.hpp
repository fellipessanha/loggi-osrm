#pragma once

#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <unordered_set>

namespace loggibud {

std::vector<std::vector<int>>
firstBadSolution(const std::vector<Delivery>& allDeliveries, const int& vehicle_cap)
{
  std::vector<std::vector<int>> routes;
  int thisCap = 0;
  std::vector<int> thisRoute;
  for (int i = 0; i < allDeliveries.size(); i++) {
    const Delivery& d = allDeliveries[i];
    if (thisCap + d.size > vehicle_cap) {
      routes.push_back(thisRoute);
      thisRoute.clear();
      thisCap = 0;
    }
    thisCap += d.size;
    thisRoute.push_back(i);
  }
  if (thisRoute.size())
    routes.push_back(thisRoute);

  return routes;
}
std::vector<std::vector<int>>
pucaClusterGenerator(
  Instance& instance,
  const std::unordered_map<size_t, std::unordered_map<size_t, double>> distMap,
  const int& vehicle_cap)
{
  const auto& allDeliveries = instance.getAllDeliveries();
  const auto& originDists = instance.getdistsFromOrigin();
  // std::cout << "\nPUCA CLUSTER GENERATOR\n";
  //
  // return variable
  std::vector<std::vector<int>> routes;
  //
  // Where we the stops without allocated
  std::unordered_set<size_t> possible;
  for (auto it = distMap.begin(); it != distMap.end(); ++it) {
    possible.insert(it->first);
  }
  // 0 has to be in every map because every stop may be adjacent the deposit.
  // We have to remove it here, or the program will try to input it twice every route.
  possible.erase(0);

  while (possible.size()) {

    // A route has a thisRoute vector and thisCapacity int
    std::vector<int> thisRoute;
    int thisCapacity = 0;
    // The deposit is handled out of the vector, the lines below are now deprecated
    // // Every route starts with the deposit, which has index 0
    // // thisRoute.push_back(0);
    //
    size_t rng_index = rand() % possible.size();
    auto rng_key = std::next(possible.begin(), rng_index);
    //
    // Inserts random first stop into route.
    // Doing this right before each insertion proccess avoids startgin multiple routes in a small neighborhood
    thisRoute.push_back(*rng_key);
    thisCapacity += allDeliveries[*rng_key].size;
    //
    // Erases used stop from 'possible' array, as to avoid repetitions
    possible.erase(rng_key);

    // Parallel insertion algorithm also taken from Puca Thesis
    while (thisCapacity < vehicle_cap && possible.size()) {

      // std::cout << thisCapacity << '\t';
      size_t thisOrigin = thisRoute.back();
      // copying map and removing undesirable stops. can probably be improved, idk
      std::unordered_map<size_t, double> thisMap;

      for (auto it = possible.begin(); it != possible.end(); ++it) {
        thisMap[*it] = distMap.at(thisOrigin).at(*it);
      }
      //
      // std::cout << "map constructed just fine" <<std::endl;
      auto minDistance = std::min_element(
        thisMap.begin(), thisMap.end(),
        [](const auto& l, const auto& r) { return (l.second < r.second); }
      );
      //
      // std::cout << "min found just fine" <<std::endl;

      const auto& nextStop = allDeliveries[minDistance->first];
      //

      if (nextStop.size + thisCapacity <= vehicle_cap) {
        thisRoute.push_back(minDistance->first);
        thisCapacity += nextStop.size;
        possible.erase(minDistance->first);
      } else {
        break;
      }
    }
    // for (auto i: thisRoute)
    //   std::cout << i << '\t';
    // std::cout << "\n\n";
    routes.push_back(thisRoute);
    // std::cout << "\nthe pushed back route has a total dist of " << evaluateRouteDistanceClusters(thisRoute, allDeliveries, distMap, initial_routes)
    //           << "\tpossibles' size is currently: " << possible.size() << "\tsolution currently has " << routes.size() << " routes\n";
  }
  return routes;
}

std::vector<std::vector<int>>
generatefromClusters(
  Instance& instance,
  int vehicle_cap)
{
  // const auto& allDeliveries = instance.getAllDeliveries();
  const auto& distanceClusters = instance.getClusterMap();
  const auto& originDistances = instance.getdistsFromOrigin();
  // std::cout << "INITIALIZING GENERATION BY CLUSTERS\n";

  std::vector<std::vector<int>> generatedRoutes;

  for (size_t i = 0; i < distanceClusters.size(); i++) {
    auto distMap = distanceClusters.at(i);

    std::vector<std::vector<int>> thisCluster =
      pucaClusterGenerator(instance, distMap, vehicle_cap);
    // std::cout << "\nroutes.size() = " << generatedRoutes.size() << "\t routing finished just fine\n"
              // << std::endl;
    for (auto i : thisCluster) {
      generatedRoutes.push_back(i);
    }
  }
  // std::cout << "\nGENERATION COMPLETE" << std::endl;
  return generatedRoutes;
}

} // namespace

/* old Puca Generator
  std::vector<std::vector<int>>
  pucaGenerator(
    const std::vector<Delivery> &allDeliveries, 
    const int &vehicle_cap, 
    const int &initial_routes){

    std::cout << "\nPUCA GENERATOR\n";
    // return variable
    std::vector<std::vector<int>> routes;
    //
    // Where we the stops without allocated
    std::vector<int> possible(allDeliveries.size() - 1, 1);
    std::iota(possible.begin(), possible.end(), 1);

    for (int i = 0; i < initial_routes; i++)
    {
      std::cout << "routes vector has " << routes.size() << " elements\n";

      // A route has a thisRoute vector and thisCapacity int
      std::vector<int> thisRoute;
      int thisCapacity = 0;
      // Every route starts with the deposit, which has index 0
      thisRoute.push_back(0);
      //
      int rng_index = (rand() % (possible.size() - 1));
      int rng_key = possible[rng_index];
      //
      // Inserts random first stop into route
      thisRoute.push_back(rng_key);
      thisCapacity += allDeliveries[rng_key].size;
      //
      // Inserts a random stop and erases it from 'possible' array
      possible.erase(possible.begin() + rng_index);

      // Parallel insertion algorithm also taken from Puca Thesis
      std::cout << "making OSRM request\n";
      std::vector<double> dists =
          evaluateDistsfromPoint(allDeliveries, possible, thisRoute.back());
      std::cout << "OSRM request made\n";

      while (thisCapacity < vehicle_cap && possible.size())
      {
        std::cout << "this route has " << thisRoute.size() << " stops\n";

        // Gets minimum distance from last stop (can be improved!)
        std::min_element(dists.begin(), dists.end());
        size_t min_index = std::distance(
            dists.begin(), std::min_element(dists.begin(), dists.end()));

        auto nextStop = allDeliveries[possible[min_index]];

        if (nextStop.size + thisCapacity <= vehicle_cap)
        {
          thisRoute.push_back(possible[min_index]);
          thisCapacity += nextStop.size;
          possible.erase(possible.begin() + min_index);
          dists.erase(dists.begin() + min_index);
        }
        else
        {
          routes.push_back(thisRoute);
          std::cout << "\nthe pushed back route has a total dist of " << evaluateRouteDistanceOSRM(thisRoute, allDeliveries) <<"\n";
          std::cout << "possibles' size is currently: " << possible.size() << "\n";
          break;
        }
      }
    }
    return routes;
  }
  */
