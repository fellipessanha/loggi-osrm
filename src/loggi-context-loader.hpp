
#pragma once

#include <fstream>  // reading files
#include <iostream> // cout, strings
#include <utility>  // std::pair
#include <vector>   // std::vector
//
#include "../nlohmann/json.hpp"
using json = nlohmann::json;

namespace loggibud {

struct Delivery
{
  std::string id;
  std::string pt;
  int size;
  int cluster;
  std::pair<double, double> point; // in format  {lng, lat}
};

class Instance
{
  //
  std::string outPath;
  //
  std::string name;
  std::string region;
  int max_hubs;
  int vehicle_capacity;
  std::pair<double, double> origin_coords; // in format  {lng, lat}
  std::string origin;                      // in format $lng+';'+$lat
  //
  std::vector<Delivery> deliveries;
  std::vector<int> clusterList;
  size_t n_clusters;
  //
  // distancesFromOrigin[stop].first  -> from origin to point
  // distancesFromOrigin[stop].second -> from point to origin
  std::vector<std::pair<double, double>> distancesFromOrigin;
  //
  std::vector<std::unordered_map<size_t, std::unordered_map<size_t, double>>>
    clusterDistances;
  //
  std::vector<int> cluster_of_specific_route;
  std::vector<std::vector<int>> routes_by_cluster;

  void loadDetails(json jInstance, json jDelivery)
  {
    // Auxiliar text things
    std::stringstream text_coords;
    text_coords.precision(15); // make it as precise as you want!
    //
    std::cout << "instance details...\t";
    name = jInstance["name"];
    //
    std::pair<double, double> coords;
    coords.first = jInstance["origin"]["lng"];
    coords.second = jInstance["origin"]["lat"];
    origin_coords = coords;

    text_coords << coords.first << ',' << coords.second;
    origin = text_coords.str(); // std::to_string(coords.first) + ',' + std::to_string(coords.second);
    //
    vehicle_capacity = jInstance["vehicle_capacity"];
    //
    std::cout << "ok!\nloading individual deliveries...\t";
    int counter = 0;
    for (json i : jInstance["deliveries"]) {
      text_coords.str("");
      Delivery d;
      d.id = i["id"];
      d.size = i["size"];
      coords.first = i["point"]["lng"];
      coords.second = i["point"]["lat"];
      text_coords << coords.first << ',' << coords.second;
      d.pt = text_coords.str(); // std::to_string(coords.first) + ',' + std::to_string(coords.second);
      deliveries.push_back(d);
      counter++;
    }
    std::cout << "ok!\nsuccesfully loaded " << counter << " delivery points!\n";
  }

  void loadClusters(const json& jClusterData)
  {
    int max_cluster = 0;
    for (size_t i = 0; i < deliveries.size(); i++) {
      Delivery& del = deliveries[i];
      std::string id = del.id;
      //
      int clusterN = jClusterData[id];
      //
      del.cluster = clusterN;
      //
      clusterList.push_back(clusterN);
      assert(deliveries[i].cluster == clusterList[i]);
      max_cluster = max(max_cluster, clusterN);
    }
    assert(deliveries.size() == clusterList.size());
    n_clusters = max_cluster;
  }

public:
  void makeDistancesFromOrigin();

  const int getClusterN() const
  {
    return n_clusters;
  }

  const int getCap() const
  {
    return vehicle_capacity;
  }

  const std::vector<Delivery>& getAllDeliveries() const
  {
    return deliveries;
  }

  const int capacity()
  {
    return vehicle_capacity;
  }

  const std::vector<int>& getClusters() const
  {
    return clusterList;
  }

  const int getClusters(size_t stop) const
  {
    return clusterList[stop];
  }

  const std::string& getOrigin() const
  {
    return origin;
  }

  // TODO: desgambiarrar
  const std::vector<std::pair<double, double>>& getdistsFromOrigin() const
  {
    return distancesFromOrigin;
  }

  const double& distFromOrigin(size_t stop) const
  {
    return distancesFromOrigin[stop].first;
  }

  const double& distToOrigin(size_t stop) const
  {
    return distancesFromOrigin[stop].second;
  }

  const std::unordered_map<size_t, std::unordered_map<size_t, double>>&
  getClusterMap(size_t cluster)
  {
    return clusterDistances.at(cluster);
  }

  const std::vector<std::unordered_map<size_t, std::unordered_map<size_t, double>>>&
  getClusterMap()
  {
    return clusterDistances;
  }

  std::vector<int>& get_clusters_by_route_vector()
  {
    return cluster_of_specific_route;
  }

  const int get_cluster_of_route(size_t route) const
  {
    return cluster_of_specific_route[route];
  }

  const std::vector<int>& get_routes_in_cluster(size_t cluster) const
  {
    return routes_by_cluster[cluster];
  }

  std::vector<std::vector<int>>& ref_routes_by_cluster()
  {
    // std::vector<std::vector<int>> aux_routes_by_cluster(n_clusters);
    // for (int route_idx = 0; route_idx < cluster_of_specific_route.size(); route_idx++) {
    //   aux_routes_by_cluster[cluster_of_specific_route[route_idx]].push_back(route_idx);
    // }
    // routes_by_cluster = aux_routes_by_cluster;
    return routes_by_cluster;
  }

  void makeMatrixDistances();

  Instance(const std::string& instancePath, const std::string& deliveryPath, const std::string& clusterPath = "")
  {
    std::cout << "loading files:\n";
    json _instance, _delivery, _clusters;
    //
    // Using this single ifstream to read all the files.
    // Must remember to close it after using each time, or else it breaks
    std::ifstream reader(instancePath, std::ifstream::in);
    //

    std::cout << "instance...\t";
    reader >> _instance;
    reader.close();
    std::cout << "loaded!\n";
    //
    std::cout << "delivery...\t";
    reader.open(deliveryPath, std::ifstream::in);
    reader >> _delivery;
    std::cout << "loaded!\nstarting Instance constructor...\n";
    reader.close();
    //

    loadDetails(_instance, _delivery);

    if (clusterPath == "") {
      std::cout << "no clusters to read\n";
    } else {
      std::cout << "loading clusters from \"" << clusterPath << "\"...\t" << std::flush;
      reader.open(clusterPath, std::ifstream::in);
      // std::cout << reader.rdbuf() << '\n' << std::endl;
      reader >> _clusters;
      loadClusters(_clusters);
      makeDistancesFromOrigin();
      makeMatrixDistances();
      std::cout << "done!";
    }
  }
};

//#include "MoveData.hpp"

} //namespace loggibud