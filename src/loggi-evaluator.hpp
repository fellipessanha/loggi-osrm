namespace loggibud{

  std::vector<double> evaluateDistsfromPoint
  (const std::vector<Delivery> &allDeliveries, const std::vector<int> &viableOptions, int source_index = 0){
    
    // Not sure if that's how i works, but cleaning up our http stuff here
    curlpp::Cleanup myCleanup;
    //
    // preparing the url that's gon be used
    std::stringstream ss;
    // ss.precision(4);
    ss << "localhost:5000/table/v1/driving/" << allDeliveries[viableOptions[source_index]].pt;
    // ss << "http://router.project-osrm.org/table/v1/driving/" << allDeliveries[viableOptions[source_index]].pt;
    // TODO: test to see how OSRM deals with it!
    size_t usedOptions = viableOptions.size();
    for (size_t i = 0; i < usedOptions; i++){
      ss << ';' << allDeliveries[viableOptions[i]].pt;
    }
    // Same options as in loggibud documentation
    ss << "?sources=0";
    //
    // curlpp easymode fun
    curlpp::Easy myRequest;
    myRequest.setOpt<curlpp::options::Url>(ss.str());
    // after setting URL we can clear stringstream
    ss.str("");
    // making the request write straight to the ss stringstream avoids cluttering the cout
    myRequest.setOpt(curlpp::options::WriteStream(&ss));
    //
    try{
      myRequest.perform();
    }
    catch(curlpp::RuntimeError & e){
      std::cout << e.what() << std::endl;
    }

    // setting up the json file
    // std::cout << "\n" << ss.str() << "\n";
    json osrmOut;
    ss >> osrmOut;

    std::vector<double> outputs;
    for (size_t i = 0; i < usedOptions; i++){
      outputs.push_back(osrmOut["durations"][0][i]);
    }   
    return outputs;
  }


  double evaluateRouteDistance
  (const std::vector<int> &route, const std::vector<Delivery> &allDeliveries){

    // Not sure if that's how i works, but cleaning up our http stuff here
    curlpp::Cleanup myCleanup;
    //
    // preparing the url that's gon be used
    std::stringstream ss;
    ss.precision(4);
    ss << "localhost:5000/route/v1/driving/" << allDeliveries[route[0]].pt;
    // ss << "http://router.project-osrm.org/route/v1/driving/" << allDeliveries[route[0]].pt;
    // TODO: test to see how OSRM deals with it!
    for (int i = 1; i < route.size(); i++){
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
    try{
      myRequest.perform();
    }
    catch(curlpp::RuntimeError & e){
      std::cout << e.what() << std::endl;
    }
    
    // setting up the json file
    json osrmOut;
    ss >> osrmOut;
    //
    // returning sum of all distances
    return osrmOut["routes"][0]["distance"];
  }

  double evaluateInstance 
  (const std::vector<std::vector<int>> &routes, const std::vector<Delivery> &allDeliveries){
    double instanceDistance = 0;
    for (auto i: routes){
      double thisdist = evaluateRouteDistance(i, allDeliveries);
      // std::cout << thisdist << '\t';
      instanceDistance += thisdist;
    }
    return instanceDistance;
  }
} // namespace