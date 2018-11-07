all: cachetest_hw3

cachetest_hw3: cache_test_hw3.o cache.o evictors.o
	g++ -o cachetest_hw3 cache_test_hw3.o cache.o evictors.o -Wall -Werror -Wextra -pedantic

cache_test_hw3.o: cache_test_hw3.cpp
	g++ -c cache_test_hw3.cpp -Wall -Werror -Wextra -pedantic

cache.o: cache.cpp evictors.o
	g++ -c cache.cpp -Wall -Werror -Wextra -pedantic

evictors.o: evictors.cpp
	g++ -c evictors.cpp -Wall -Werror -Wextra -pedantic

clean: 
	rm -f *.o