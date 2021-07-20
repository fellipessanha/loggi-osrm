#include "loggi-include.hpp"

int main()
{
	loggibud::Instance instance("data/cvrp-0-rj-0.json", "data/rj-0.json");
	auto &deliveriesList = instance.getAllDeliveries();
	auto routes = loggibud::firstBadSolution(deliveriesList, instance.capacity());
	int nofEval = 3;
	// for (int i = 0; i < nofEval; i++)
	// 	double evaluation = loggibud::evaluateInstance(routes, deliveriesList);
	
	// std::cout << "total evaluation = " << evaluation;

	std::cout << "testing with routes[0]: " << routes[5].size() << "\n";

	/*
	auto test = loggibud::evaluateDistsfromPoint(deliveriesList, routes[5], routes[0][0]);
	for (auto i : test)
		std::cout << i << ", ";
	*/
	
	auto newRoutes = loggibud::pucaGenerator(deliveriesList, instance.capacity(), routes.size());



	// std::cout << '\n';
	return 0;
}