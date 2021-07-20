namespace loggibud {

struct Delivery
{
  std::string id;
  std::string pt;
  int size;
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
  //
  void loadDetails(json jInstance, json jDelivery)
  {
    // Auxiliar text things
    std::stringstream text_coords;
    text_coords.precision(7); // make it as precise as you want!
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
    for (json i : jInstance["deliveries"])
    {
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
    std::cout << "succesfully loaded " << counter << " delivery points!\n";
  }

public:
  const int getCap() {return vehicle_capacity;}
  const std::vector<Delivery> &getAllDeliveries()
  {
    return deliveries;
  }

  const int capacity(){
      return vehicle_capacity;
    }

  Instance
  (const std::string &instancePath, const std::string &deliveryPath, const std::string &outPath_= ".")
  : outPath(outPath_)
  {
    std::cout << "loading files:\n";
    json _instance, _delivery;
    //
    std::cout << "instance...\t";
    std::ifstream reader(instancePath, std::ifstream::in);
    reader >> _instance;
    reader.close();
    std::cout << "loaded!\n";
    //
    std::cout << "delivery...\t";
    reader.open(deliveryPath, std::ifstream::in);
    reader >> _delivery;
    std::cout << "loaded!\nstarting Instance constructor...\n";
    loadDetails(_instance, _delivery);
  }

};

} //namespace