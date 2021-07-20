#include "loggi-include.hpp"

int main()
{
  // instance loading
  loggibud::Instance instance("data/cvrp-0-rj-0.json", "data/rj-0.json");
  auto &deliveriesList = instance.getAllDeliveries();
  loggibud::FCoreLoggi fC_loggi(instance);



 

  return 0;
}