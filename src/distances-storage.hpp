#pragma once

#include <vector>
#include "loggi-context-loader.hpp"
#include "loggi-evaluator.hpp"
#include <algorithm>
#include <map>

namespace loggibud{

// I had to store it in maps because the size is no longer uniform
// We should always call a stop by their position in the Deliveries vector
// Call by map[cluster_number][x][y], where x and y are destination or origin
std::vector<std::unordered_map<size_t, std::unordered_map<size_t, double>>>
makeMatrixDistances(Instance inst){
  const std::vector<int> clusters = inst.getClusters();
  int n_clusters = *std::max_element(clusters.begin(), clusters.end())+1;

  const auto deliveries = inst.getAllDeliveries();
  std::vector<std::vector<int>> clusterMap(n_clusters, {0});
  std::cout << "\n" << deliveries.size()<< '\t' << n_clusters << '\n';


  for (int i = 0; i < deliveries.size(); i++){
    // std::cout << i << ": " << clusters[i] <<'\t' << std::flush;
    clusterMap[clusters[i]].push_back(i);
    assert(clusters[i] == deliveries[i].cluster);
  }
  //
  std::vector<std::unordered_map<size_t, std::unordered_map<size_t, double>>>
  distances;
  //
  for (size_t cluster = 0; cluster < n_clusters; cluster++){
    const std::vector<int>& thisCluster = clusterMap[cluster];
    auto matrix = evaluateDistsMatrix(thisCluster, deliveries);

    std::unordered_map<size_t, std::unordered_map<size_t, double>> dist_map;
    for (size_t row = 0; row < matrix.size(); row++){
      for (size_t column = 0; column < matrix.size(); column++){      
        dist_map[thisCluster[row]][thisCluster[column]] = 
          matrix[row][column];
      };
    }
    for (auto k: dist_map)
      assert(deliveries[k.first].cluster == clusters[k.first]);
    distances.push_back(dist_map);
  }
  return distances;
}

} // namespace loggibud
