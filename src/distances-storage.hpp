#pragma once

#include "loggi-context-loader.hpp"
#include "loggi-evaluator.hpp"
#include <algorithm>
#include <map>
#include <vector>

namespace loggibud {

// I had to store it in maps because the size is no longer uniform
// We should always call a stop by their position in the Deliveries vector
// Call by map[cluster_number][x][y], where x and y are destination or origin
void Instance::makeMatrixDistances()
{
  // const std::vector<int> clusterList = inst.getClusters();
  int n_clusters = *std::max_element(clusterList.begin(), clusterList.end()) + 1;

  // const auto deliveries = inst.getAllDeliveries();
  std::vector<std::vector<int>> clusterMap(n_clusters, { 0 });
  std::cout << "\n"
            << deliveries.size() << '\t' << n_clusters << '\n';

  for (int i = 0; i < deliveries.size(); i++) {
    // std::cout << i << ": " << clusters[i] <<'\t' << std::flush;
    clusterMap[clusterList[i]].push_back(i);
    assert(clusterList[i] == deliveries[i].cluster);
  }
  //
for (size_t cluster = 0; cluster < n_clusters; cluster++) {
    const std::vector<int>& thisCluster = clusterMap[cluster];
    auto matrix = evaluateDistsMatrix(thisCluster, deliveries);

    std::unordered_map<size_t, std::unordered_map<size_t, double>> dist_map;
    for (size_t row = 0; row < matrix.size(); row++) {
      for (size_t column = 0; column < matrix.size(); column++) {
        dist_map[thisCluster[row]][thisCluster[column]] =
          matrix[row][column];
      };
    }
    for (auto k : dist_map)
      assert(deliveries[k.first].cluster == clusterList[k.first]);
    clusterDistances.push_back(dist_map);
  }
  std::cout << "\n\nclusterDistances Modularization worked!\n\n";
}

// std::vector<std::pair<double, double>>
// std::vector<std::pair<double, double>>
void Instance::makeDistancesFromOrigin()
{
  size_t sz = deliveries.size();
  std::vector<std::pair<double, double>> originDistances;

  std::string stopsPts = deliveries[0].pt;
  for (size_t i = 1; i < sz; i++)
    stopsPts += ';' + deliveries[i].pt;


  auto toOrigin = evaluateDistsfromPointString(stopsPts, origin, true);
  auto fromOrigin = evaluateDistsfromPointString(stopsPts, origin, false);

  std::cout << "totalDeliveries: " << sz << "\ttoOrigin: " << toOrigin.size() << "\tfromOrigin: " << fromOrigin.size() << '\n'; 

  for(size_t stop = 0; stop < sz; stop++){
    distancesFromOrigin.push_back( std::make_pair(fromOrigin[stop], toOrigin[stop]) );
  }
  // std::cout << originDistances.size() << '\n';
}

} // namespace loggibud
