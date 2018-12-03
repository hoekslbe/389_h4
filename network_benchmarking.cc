#include cache.hh
#include <iostream>
#include <chrono>
#include <cassert>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>



duration<double, nano> measure_total_time(Cache &cache, 
										std::vector<std::string> &requests, 
										std::vector<std::string> &keys, 
										std::vector<void*> &vals, 
										std::vector<Cache::index_type> &sizes) {
	assert(requests.size() == keys.size());
	assert(keys.size() == vals.size());
	assert(vals.size() == sizes.size());
	std::string request;
	unsigned get_size;

	const auto start_time = steady_clock::now();

	for (int i = 0; i < requests.size(); ++i) {
		request = requests[i];
		if (request == "GET") {
			out = cache.get (keys[i], get_size);
			operator delete(out, get_size);
		
		} else if (request == "SET") {
			cache.set(keys[i], vals[i], sizes[i]);
		
		} else if (request == "DEL") {
			cache.del(keys[i]);
			
		} else if (request == "SPA") {
			cache.space_used();
		}
	}

	const auto end_time = steady_clock::now();

	duration<double, nano> elapsed_time = end_time - start_time;  
	return elapsed_time;
}

void assemble_requests_to_measure(std::vector<std::string> &requests, 
									std::vector<std::string> &keys, 
									std::vector<void*> vals,
									std::vector<Cache::index_type &sizes, 
									const std::vector<float> &distribution, 
									const unsigned num_requests) {
	assert(distribution.size() == 4);
	sum
	assert()
	for (unsigned i = 0; i < num_requests, ++i) {
		float v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		int i = 0;
		while i
	}
	
}


// what command line arguments will we need?

int main() {
	srand(time(NULL));
}