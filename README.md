# loggi-osrm research:

# Inside src/

## cpp files:

curlpp-example.cpp is reference I use to remember how to use curlpp and osrm http requests

main-loggi.cpp was used to test the different algorithms

optframe-main.cpp is used to call optframe and actually try to optimize the problem.

## hpp files:

- loggi-include.hpp calls necessary functions and every other hpp in the correct order.

- loggi-context-loader.hpp has all the main class definitions regarding the problem itself.

- loggi-evaluator.hpp has functions that call OSRM to evalute distances

- loggi-solution-generators.hpp has the initial solutions. Currently a dumb one that generates on the giver order of the .json file and another that was based on the puca interpretation of the Clarke-Wright algorithm

- FCore_functions.hpp have the functions that will be used in [OptFrame](http://github.com/optframe/optframe/)'s FCore.

# Inside data/

A sample from [loggibud](http://github.com/loggi/loggibud/)'s data
