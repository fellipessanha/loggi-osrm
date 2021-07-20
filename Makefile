all: curlExample evalloggi contextloggi generatorloggi osrm-server main-loggi

curlExample:
	g++ src/curlpp-example.cpp -o curlExample -lcurl -lcurlpp

main-loggi:
	g++ src/main-loggi.cpp -o main-loggi -lcurl -lcurlpp 

osrm-server:
	docker run --rm -ti -p 5000:5000 -v "${PWD}/sudeste-osrm:/data" osrm/osrm-backend osrm-routed --algorithm ch /data/sudeste-latest.osrm
# evalloggi:
# 	g++ src/loggi-evaluator.hpp -lcurl -lcurlpp -o evalloggi

# generatorloggi:
# 	g++ src/loggi-solution-generators.cpp -lcurl -lcurlpp -o generatorloggi

