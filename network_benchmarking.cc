#include "cache.hh"
#include <iostream>
#include <chrono>
#include <cassert>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>


const unsigned GET_PROPORTION = 0; // these shouldn't be 0.  Figure out what they should be according to the memcached workload.  
const unsigned SET_PROPORTION = 0;
const unsgiend DEL_PROPORTION = 0;
const unsigned SPA_PROPORTION = 0;


struct KeyValueStore { // not a map or anything - just stores keys + values, and hands them out on request.  Handles the deletion of the values it stores.  
	KeyValueStore(const std::vector<std::string> &keys = nullptr, const std::vector<std::string> &values = nullptr) {
		if (keys) {
			for (std::string key : keys) {
				add_key(key);
			}
		}
		if (values) {
			for (std::string value_string : values) {
				add_value(value_string);
			}
		}
		key_index_ = 0;
		value_index_ = 0;
	}
	~KeyValueStore() {
		for (std::pair<void*, unsigned> val_size_pair : values_) {
			operator delete(val_size_pair.first, val_size_pair.second);
		}
	}
	
	void add_key(std::string key) {
		unsigned size = 0;
		void* check_val = string_to_val(key, size);
		assert(size == key.size() && "There was something wrong with the key you passed in")
		operator delete(check_val, size);
		keys_.push_back(key);
	}

	std::string get_key () {
		std::string key = *(keys_[key_index_]);
		key_index = (key_index_ + 1) % keys_.size();
		return key;
	}

	void add_value(std::string value_string) {
		unsigned size = 0;
		void* val = string_to_val(value_string, size);
		assert(size == value_string.size() && "There was something wrong with the string you passed in");
		values_.push_back(std::make_pair(value, size));
	}

	void* get_value (unsigned &size) {
		std::pair<void*, unsigned> *val_size_pair = values_[value_index_];
		value_index = (value_index_ + 1) % values_.size();
		size = val_size_pair->second;
		return val_size_pair->first;
	}
	std::vector<std::string> keys_;
	std::vector<std::pair<void*, unsigned> > values_;
	unsigned value_index_;
	unsigned key_index_;
};

struct RequestDistribution { // stores a probability distribution over our different request types - 'GET', 'SET', 'DEL', and 'SPA' (spaced_used).  Gives requests according to the distribution with get_request.  

	RequestDistribution (const unsigned GET_factor = 0, const unsigned SET_factor = 0, const unsigned DEL_factor = 0, const unsigned SPA_factor = 0) {
		
		float mod_GET_factor = static_cast<float>(GET_factor);
		float mod_SET_factor = mod_GET_factor + static_cast<float>(SET_factor);
		float mod_DEL_factor = mod_SET_factor + static_cast<float>(DEL_factor);
		float mod_SPA_factor = mod_DEL_factor + static_cast<float>(SPA_factor);
		
		internal_distribution_.push_back(std::make_pair("DEL", mod_DEL_factor / mod_SPA_factor));
		internal_distribution_.push_back(std::make_pair("SET", mod_SET_factor / mod_SPA_factor));
		internal_distribution_.push_back(std::make_pair("GET", mod_GET_factor / mod_SPA_factor));
		internal_distribution_.push_back(std::make_pair("SPA", mod_SPA_factor / mod_SPA_factor));

		assert(internal_distribution_.size() == 4 && "Something went wrong - unexpected number of items in the probability distribution");
		for (unsigned i = 0; i < internal_distribution_.size()-1; i++) {
			assert(distribution[i]->second < internal_distribution_[i+1]->second && "something went wrong - distribution was ill-formed");
		}
		assert(internal_distribution_.back.second >= 1.0 && "Something went wrong - total probability mass in distribution was less than 1");

	}

	~RequestDistribution () {}

	const std::string get_request() {
		std::string r_val;
		float v = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		for (std::pair<std::string, float> request_prob_pair : internal_distribution_) {
			if (request_prob_pair.second >= v) {
				r_val = request_prob_pair.first;
				break;
			}
		}
		return r_val;
	}

	std::vector<std::pair<std::string, std::float> > internal_distribution_;
};

struct TestParameters {

	std::vector<std::string>* get_requests
}

duration<double, nano> measure_total_time(Cache &cache, 
										const std::vector<std::string> &requests, 
										const std::vector<std::string> &keys, 
										const std::vector<void*> &vals, 
										const std::vector<Cache::index_type> &sizes) {
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


//make a struct to store these inputs

void assemble_requests_to_measure(std::vector<std::string> &requests, 
									std::vector<std::string> &keys, 
									std::vector<void*> vals,
									std::vector<Cache::index_type &sizes, 
									KeyValueStore &kvs,
									const RequestDistribution &distribution, 
									const unsigned num_requests) {
	assert(requests.size() == 0 && "requests vector was not empty");
	assert(keys.size() == 0 && "keys vector was not empty");
	assert(vals.size() == 0 && "vals vector was not empty");
	assert(sizes.size() == 0 && "sizes vector was not empty");
	
	for (unsigned i = 0; i < num_requests, ++i) {
		std::string request = distribution.get_request();
		requests.push_back(request);
		if (request == "GET") {
			//add a key to keys
			keys.push_back(kvs.get_key());
			vals.push_back(nullptr);
			sizes.push_back(0);

		} else if (request == "SET") {
			keys.push_back(kvs.get_key());
			unsigned get_size;
			vals.push_back(kvs.get_val(get_size));
			sizes.push_back(get_size)
		
		} else if (request == "DEL") {
			keys.push_back(kvs.get_key());
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
// let's start w/ none

int main() {
	srand(time(NULL));
	//build distribution for bandwith
	//build requests vectors
	//
	// build key_value stores
	// Factors: key size (large / small), value size (large / small), number of keys (one / three / 100)
	//clean up void pointers in vals vector
	//	
	// case 1: large keys, 
	// build the vectors for latency tests

	Distribution dist(GET_PROPORTION, GET_PROPORTION, DEL_PROPORTION, SPA_PROPORTION);

	Key_Value_Store kvs_case1()


}