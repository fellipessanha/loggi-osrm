all: curlExample osrm-server main-loggi bad

curlExample:
	g++ src/curlpp-example.cpp -o curlExample -lcurl -lcurlpp

main-loggi:
	g++ -fsanitize=address -g -Wfatal-errors -std=c++17 -I../optframe/src/ -fconcepts src/main-loggi.cpp -o main-loggi -lcurl -lcurlpp 

osrm-server:
	docker run --rm -ti -p 5000:5000 -v "${PWD}/sudeste-osrm:/data" osrm/osrm-backend osrm-routed --algorithm ch /data/sudeste-latest.osrm --max-table-size 10000

bad:
	g++-10 -fsanitize=address -g -Wfatal-errors -std=c++17 -I../optframe/src/ -fconcepts src/bad-opframe-main.cpp -o main-loggi -lcurl -lcurlpp 
# evalloggi:
# 	g++ src/loggi-evaluator.hpp -lcurl -lcurlpp -o evalloggi

# generatorloggi:
# 	g++ src/loggi-solution-generators.cpp -lcurl -lcurlpp -o generatorloggi

