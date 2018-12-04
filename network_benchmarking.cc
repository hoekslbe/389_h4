#include cache.hh
#include <iostream>
#include <chrono>
#include <cassert>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>


struct KeyValueStore {
	KeyValueStore() {
		key_index_ = 0;
		value_index_ = 0;
	}
	~KeyValueStore() {}
	
	void add_key(std::string key) {
		keys_.push_back(key);
	}

	std::string get_key () {
		std::string key = *(keys_[key_index_]);
		key_index = (key_index_ + 1) % keys_.size();
		return key;
	}

	void add_value(std::string value) {
		values_.push_back(value);
	}

	std::string get_value () {
		std::string val = *(values_[value_index_]);
		value_index = (value_index_ + 1) % values_.size();
		return val;
	}
	std::vector<std::string> keys_;
	std::vector<std::string> values_;
	unsigned value_index_;
	unsigned key_index_;
};


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
									KeyValueStore &kvs,
									const std::vector<std::pair<std::string, float> > &distribution, 
									const unsigned num_requests) {
	assert(distribution.size() == 4);
	assert(distribution.back.second >= 1.0);
	for (unsigned i = 0; i < num_requests, ++i) {
		float v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		std::string request;
		for (std::pair<std::string, float> request_prob_pair : distribution) {
			if (request_prob_pair.second >= v) {
				request = request_prob_pair.first;
				break;
			}
		}
		requests.push_back(request);
		if (request == "GET") {
			//add a key to keys
			keys.push_back(kvs.get_key());
			vals.push_back(nullptr);
			sizes.push_back(0);

		} else if (request == "SET") {
			keys.push_back(kvs.get_key());
			unsigned get_size;
			vals.push_back(string_to_val(kvs.get_val(), get_size);
			sizes.push_back(get_size)
		
		} else if (request == "DEL") {
			keys.push_back something something huh?
			vals.push_back(nullptr);
			sizes.push_back(0);

			
		} else if (request == "SPA") {
			keys.push_back("");
			vals.push_back(nullptr);
			sizes.push_back(0);
		}

	}
	
}


// what command line arguments will we need?

int main() {
	srand(time(NULL));

	

}