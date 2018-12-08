#include "cache.hh"
#include "HTTP_utilities.hh"
#include <iostream>
#include <chrono>
#include <cassert>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>

// parameters for indicating request proportions according to desired workload 
const unsigned GET_PROPORTION = 1; // these shouldn't be 0.  Figure out what they should be according to the memcached workload.  
const unsigned SET_PROPORTION = 1;
const unsigned DEL_PROPORTION = 1;
const unsigned SPA_PROPORTION = 1;

/* a struct which supports a vector each for storing keys and values and functionality for adding and 
retrieiving them from storage. */
struct KeyValueStore { 
	KeyValueStore(const std::vector<std::string> &keys, const std::vector<std::string> &values) {
		for (std::string key : keys) {
			add_key(key);
		}
		for (std::string value_string : values) {
			add_value(value_string);
		}
		key_index_ = 0;
		value_index_ = 0;
	}
	// upon destruction, the existing keys and values will be erased from memory
	~KeyValueStore() {
		for (std::pair<void*, unsigned> val_size_pair : values_) {
			operator delete(val_size_pair.first, val_size_pair.second);
		}
	}
	/* adds a key by name.
	   after calling string_to_val, size will be set to the length of the c-string form of the key, 
	   but if something about the string causes its reading to be terminated prematurely (like the presence of an
	   underscore), the returned size will not match with the given key string's size. Knowing this, the call to string_to_val 
	   and subsequent assert on returned size will indicate whether the given key string can be successfully passed
	   through the network by our program. */ 
	void add_key(std::string key) {
		unsigned size = 0;
		void* check_val = string_to_val(key, size);
		assert(size == key.size() && "There was something wrong with the key you passed in");
		// delete the bytes utilized in calling check_val before adding the key 
		operator delete(check_val, size);
		keys_.push_back(key);
	}

	std::string get_key () {
		std::string key = keys_[key_index_];
		key_index_ = (key_index_ + 1) % keys_.size();
		return key;
	}
	// add a value by name, checking its validity to be passed through the network with a call to string_to_val
	void add_value(std::string value_string) {
		unsigned size = 0;
		void* val = string_to_val(value_string, size);
		assert(size == value_string.size() && "There was something wrong with the string you passed in");
		values_.push_back(std::make_pair(val, size));
	}
	// get a value to return by byte size 
	void* get_value (unsigned &size) {
		std::pair<void*, unsigned> val_size_pair = values_[value_index_];
		value_index_ = (value_index_ + 1) % values_.size();
		size = val_size_pair.second;
		return val_size_pair.first;
	}
	std::vector<std::string> keys_;
	std::vector<std::pair<void*, unsigned>> values_;
	unsigned value_index_;
	unsigned key_index_;
};

/* stores a probability distribution over our different request types - 'GET', 'SET', 'DEL', and 'SPA' (spaced_used).  
Gives requests according to the distribution with get_request. */
struct RequestDistribution {   

	RequestDistribution (const unsigned GET_factor = 0, const unsigned SET_factor = 0, const unsigned DEL_factor = 0, const unsigned SPA_factor = 0) {
		
		float mod_GET_factor = static_cast<float>(GET_factor);
		float mod_SET_factor = mod_GET_factor + static_cast<float>(SET_factor);
		float mod_DEL_factor = mod_SET_factor + static_cast<float>(DEL_factor);
		float mod_SPA_factor = mod_DEL_factor + static_cast<float>(SPA_factor);
		
		internal_distribution_.push_back(std::make_pair("GET", mod_GET_factor / mod_SPA_factor));
		internal_distribution_.push_back(std::make_pair("SET", mod_SET_factor / mod_SPA_factor));
		internal_distribution_.push_back(std::make_pair("DEL", mod_DEL_factor / mod_SPA_factor));
		internal_distribution_.push_back(std::make_pair("SPA", mod_SPA_factor / mod_SPA_factor));

		assert(internal_distribution_.size() == 4 && "Something went wrong - unexpected number of items in the probability distribution");
		for (unsigned i = 0; i < internal_distribution_.size()-1; i++) {
			//std::cout<< "comparing " << internal_distribution_[i].second  << " and " << internal_distribution_[i+1].second << "\n";
			assert(internal_distribution_[i].second <= internal_distribution_[i+1].second && "something went wrong - distribution was ill-formed");
		}
		assert(internal_distribution_.back().second >= 1.0 && "Something went wrong - total probability mass in distribution was less than 1");

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

	std::vector<std::pair<std::string, float>> internal_distribution_;
};

struct TestParameters {

	TestParameters() {}
	~TestParameters() {}

	void add_item(std::string request, std::string key, void* val, Cache::index_type size) {
		requests_.push_back(request);
		keys_.push_back(key);
		vals_.push_back(val);
		sizes_.push_back(size);
	}

	std::vector<std::string>& get_requests() {return requests_;}
	std::vector<std::string>& get_keys() {return keys_;}
	std::vector<void*>& get_vals() {return vals_;}
	std::vector<Cache::index_type>& get_sizes() {return sizes_;}
	
	std::vector<std::string> requests_;
	std::vector<std::string> keys_;
	std::vector<void*> vals_;
	std::vector<Cache::index_type> sizes_;
};

std::chrono::duration<double, std::nano> measure_total_time(Cache &cache, TestParameters &parameters) {

	std::vector<std::string> requests = parameters.get_requests();
	std::vector<std::string> keys = parameters.get_keys();
	std::vector<void*> vals = parameters.get_vals();
	std::vector<Cache::index_type> sizes = parameters.get_sizes();

	assert(requests.size() == keys.size());
	assert(keys.size() == vals.size());
	assert(vals.size() == sizes.size());
	
	std::string request;
	unsigned get_size;
	Cache::val_type out;

	const auto start_time = std::chrono::steady_clock::now();

	for (unsigned i = 0; i < requests.size(); ++i) {
		request = requests[i];
		if (request == "GET") {
			out = cache.get (keys[i], get_size);
			operator delete((void*) out, get_size);
		
		} else if (request == "SET") {
			cache.set(keys[i], vals[i], sizes[i]);
		
		} else if (request == "DEL") {
			cache.del(keys[i]);
			
		} else if (request == "SPA") {
			cache.space_used();
		}
	}

	const auto end_time = std::chrono::steady_clock::now();

	std::chrono::duration<double, std::nano> elapsed_time = end_time - start_time;  
	return elapsed_time;
}


//make a struct to store these inputs

TestParameters assemble_requests_to_measure(KeyValueStore &kvs,
									RequestDistribution &distribution, 
									unsigned num_requests) {
	TestParameters to_return;

	for (unsigned i = 0; i < num_requests; ++i) {
		std::string request = distribution.get_request();
		std::string key = "";
		void* val = nullptr;
		Cache::index_type size = 0;
		if (request != "SPA") {
			key = kvs.get_key();

		}
		if (request == "SET") {
			val = kvs.get_value(size);
		}
		to_return.add_item(request, key, val, size);
	}
	return to_return;
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

	RequestDistribution proportioned(GET_PROPORTION, SET_PROPORTION, DEL_PROPORTION, SPA_PROPORTION);
	RequestDistribution only_GET(1, 0, 0, 0);
	RequestDistribution only_SET(0, 1, 0, 0);
	RequestDistribution only_DEL(0, 0, 1, 0);
	RequestDistribution only_SPA(0, 0, 0, 1);

	return 1;
}
