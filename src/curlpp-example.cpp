#include <iostream>
#include <sstream>
#include <fstream>
//
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
//
#include "../nlohmann/json.hpp"
using json = nlohmann::json;
//
#include <unistd.h>
//
using namespace curlpp::options;

std::stringstream ss;
// std::stringstream ss;

int main(int, char **)
{
	std::ifstream fbuff("rj-0.json", std::ifstream::in);
	json deliveries;
	fbuff >> deliveries;
	fbuff.close();
	
	deliveries = deliveries["deliveries"];
	int sz = deliveries.size();

	std::stringstream urlin;
	urlin << "localhost:5000/route/v1/driving/";
	//
	for (int j = 0; j < 2; j++){

		for (int i = 1; i < j+250; i++){
				double lng = deliveries[i-1]["point"]["lng"];
				double lat = deliveries[i-1]["point"]["lat"];

				double tolng = deliveries[i]["point"]["lng"];
				double tolat = deliveries[i]["point"]["lat"];
				urlin.precision(10);
				urlin << lng << ',' << lat << ';' << tolng << ',' << tolat << ';';
			}
			std::cout << "requesting " << sz/100 << " things in same docker url\n";
			// std::cout << urlin.str();
				
			try{
				// That's all that is needed to do cleanup of used resources (RAII style).
				curlpp::Cleanup myCleanup;

				// Our request to be sent.
				curlpp::Easy myRequest;
				
				// non-curlpp tests
				json jLeitor;

				std::string inside = urlin.str();
				inside.pop_back();
				// Set the URL.
				myRequest.setOpt<Url>(inside);
				// writing the request straigt to specific stringstreamss)
				myRequest.setOpt(curlpp::options::WriteStream(&ss));
				// Send request and get a result.
				myRequest.perform();

				ss << myRequest;

				// std::cout << ss.str();
				std::ofstream writer("algo.json", std::ofstream::out);
				writer << ss.str();
				writer.close();
				ss >> jLeitor;

				auto sla = jLeitor["routes"][0]["distance"];
				std::cout << sla << std::endl;
				// double distancinha = 0;
				// int counter = 0;
				// double lastdist = jLeitor["waypoints"][0]["distance"];
				// for (int i = 0; i < 499; i++){
				// 	double thisdist = jLeitor["waypoints"][i]["distance"];
				// 	if (thisdist == lastdist) counter++;
				// 	distancinha += thisdist;
				// 	lastdist = thisdist;
				// }

				// std::cout << "\n\nSoma das distancias: " << distancinha << "\nrepetidos: " << counter;
			}

		catch(curlpp::RuntimeError & e)
		{
			std::cout << e.what() << std::endl;
			// break;
		}

		catch(curlpp::LogicError & e)
		{
			std::cout << e.what() << std::endl;
			// break;
		}
}
  std::cout << "\n\n";
  return 0;
}