#include "loggi-include.hpp"

#include "FCore_functions.hpp"

int
main()
{
   // The case with CTAD - Class Template Argument Deduction (C++20)
   //std::vector ints{ 1, 2, 3, 4, 5, 6, 7 }; // deduced vector<int>

   // instance loading
   loggibud::Instance instance("data/cvrp-0-rj-0.json", "data/rj-0.json");
   auto& deliveriesList = instance.getAllDeliveries();
   loggibud::FCoreLoggi fC_loggi(instance);
   
   // auto sol = *fC_loggi.bad_optFC_generator.generateSolution(0);
   auto sol = *fC_loggi.optFC_generator.generateSolution(0);


   for (size_t i= 0; i < sol.size(); i++){
      std::cout << "\n\nvehicle " << i << ":\n";
      for (auto j : sol[i]){
         std::cout << j << "\t";
      }
   }
   std::cout << '\n';

   
   return 0;
}