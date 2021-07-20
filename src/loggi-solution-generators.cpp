namespace loggibud
{

  std::vector<std::vector<int>>
  firstBadSolution(const std::vector<Delivery> &allDeliveries, const int &vehicle_cap)
  {
    std::vector<std::vector<int>> routes;
    int thisCap = 0;
    std::vector<int> thisRoute;
    for (int i = 0; i < allDeliveries.size(); i++)
    {
      const Delivery &d = allDeliveries[i];
      if (thisCap + d.size > vehicle_cap)
      {
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
      // std::cout << "routes vector has " << routes.size() << " elements\n";
      // A route has a thisRoute vector and thisCapacity int
      std::vector<int> thisRoute;
      int thisCapacity = 0;
      // Every route starts with the deposit, which has index 0
      thisRoute.push_back(0);
      //
      int rng_index = (rand() % (possible.size() - 1));
      int rng_key = possible[rng_key];
      //
      // Inserts random first stop into route
      thisRoute.push_back(rng_key);
      thisCapacity += allDeliveries[rng_key].size;
      //
      // Inserts a random stop and erases it from 'possible' array
      possible.erase(possible.begin() + rng_index);

      // Parallel insertion algorithm also taken from Puca Thesis
      std::vector<double> dists =
          evaluateDistsfromPoint(allDeliveries, possible, thisRoute.back());
      while (thisCapacity < vehicle_cap && possible.size())
      {
        // std::cout << "this route has " << thisRoute.size() << " stops\n";

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
          std::cout << "\nthe pushed back route has a total dist of " << evaluateRouteDistance(thisRoute, allDeliveries) <<"\n";
          std::cout << "possibles' size is currently: " << possible.size() << "\n";
          break;
        }
      }
    }
    return routes;
  }
} // namespace
