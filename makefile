all: cachetest_hw4 server benchmarking_tests

benchmarking_tests: network_benchmarking.o client.o HTTP_utilities.o
	g++ -o benchmarking_tests network_benchmarking.o client.o HTTP_utilities.o -Wall -Werror -Wextra -pedantic

cachetest_hw4: cache_test_hw4.o client.o HTTP_utilities.o
	g++ -o cachetest_hw4 cache_test_hw4.o client.o HTTP_utilities.o -Wall -Werror -Wextra -pedantic

server: serversockets.o cache.o evictors.o HTTP_utilities.o
	g++ -g3 -o server serversockets.o cache.o evictors.o HTTP_utilities.o -Wall -Werror -Wextra -pedantic

network_benchmarking.o:
	g++ -c network_benchmarking.cc -Wall -Wextra -Werror -pedantic

serversockets.o: serversockets.cpp
	g++ -c serversockets.cpp -Wall -Werror -Wextra -pedantic

client.o: client.cc
	g++ -c client.cc -Wall -Werror -Wextra -pedantic

HTTP_utilities.o: HTTP_utilities.cc
	g++ -c HTTP_utilities.cc -Wall -Werror -Wextra -pedantic

cache_test_hw4.o: cache_test_hw4.cpp
	g++ -g3 -c cache_test_hw4.cpp -Wall -Werror -Wextra -pedantic

cache.o: cache.cpp evictors.o
	g++ -c cache.cpp -Wall -Werror -Wextra -pedantic

evictors.o: evictors.cpp
	g++ -c evictors.cpp -Wall -Werror -Wextra -pedantic

clean: 
	rm -f *.o