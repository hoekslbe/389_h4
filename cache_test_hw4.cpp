#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <vector>
#include <utility>

#include "cache.hh"

#include "HTTP_utilities.hh"

#include <iostream>

#include <memory>

// This object handles teardown for our tests.  In each test case, we create an instance, and pass it the pointers
// which will need to be deleted after the test.  When the Cleaner goes out of scope, its destructor deletes all
// the pointers we passed to it.  
// This is to avoid un-freed memory hanging around after a failed REQUIRE in testing.  
struct Cleaner {
    Cleaner(){}
    ~Cleaner(){
        while (not items_.empty()) {
            auto to_remove = items_.back();
            operator delete((void*) to_remove.first, to_remove.second);
            items_.pop_back();
        }
    }
    void add (Cache::val_type val, Cache::index_type size) {
        items_.push_back(std::make_pair(val, size));
    }
    std::vector<std::pair<Cache::val_type, Cache::index_type>> items_;
};

struct CacheHolder { // a struct to access the single cache we can create for these tests
	static std::shared_ptr<Cache> cache_pointer; //input doesn't matter here
	static bool initialized; //should be false; I can't seem to change it
	static std::shared_ptr<Cache> getCachePointer() {
		if (!initialized) {
			cache_pointer = std::make_shared<Cache>(1);
			initialized = true;
		}
		return cache_pointer;
	}
};

std::shared_ptr<Cache> CacheHolder::cache_pointer = 0;
bool CacheHolder::initialized = false;

TEST_CASE ("Testing GET"){
    // A cleaner is always created to handle the pointers needed for a test
    Cleaner cleanup;

    // Create keys and values that can be used to populate the cache
    // And create the cache itself before running tests
    Cache::key_type key = "cachekey";


	std::string s1 = "message";
    Cache::index_type size = 1;
	Cache::val_type val = string_to_val(s1, size);
    cleanup.add(val, size);

	std::string s2 = "longer_message";
    Cache::index_type size2 = 1;
    Cache::val_type val2 = string_to_val(s2, size2);
    cleanup.add(val2, size2);

    Cache::index_type get_size = 1;

	std::shared_ptr<Cache> cacheP = CacheHolder::getCachePointer();

    //Cache cache(size + size2 + 1);
    SECTION ("Getting an item present in the cache returns the item") {
		std::cout<<"hi\n";
        //cache.set(key, val, size);
		cacheP->set(key, val, size);
		std::cout<<"whi\n";
        Cache::val_type out = cacheP->get(key, get_size);
		std::cout<<"bye\n";
		std::cout<<out<<"\n";
		if (out != nullptr) {cleanup.add(out, get_size);}
		std::cout<<"wumba\n";
		cacheP->del(key);
		std::cout<<"afterdel\n";
        REQUIRE(val_to_string(out, get_size) == s1);
		
    }

    SECTION ("Get sets valsize to the size of the returned object") {
		std::cout<<"mumba\n";
        cacheP->set(key, val, size);
		std::cout<<"mumbo\n";
        Cache::val_type out = cacheP->get(key, get_size);
		std::cout<<"jumbo\n";
		if (out != nullptr) {cleanup.add(out, get_size);}
		cacheP->del(key);
        REQUIRE(get_size == size);
		
    }

    SECTION ("Get doesn't have a hard-coded val_size to return") {
        cacheP->set(key, val2, size2);
        Cache::val_type out = cacheP->get(key, get_size);
		if (out != nullptr) {cleanup.add(out, get_size);}
		cacheP->del(key);
        REQUIRE(get_size == size2);
		
    }

    SECTION ("Getting an item not in the cache returns nullptr") {
        // calls get without ever setting
        Cache::val_type out = cacheP->get(key, get_size);
		if (out != nullptr) {cleanup.add(out, get_size);}
		std::cout<<"Howabunga nowabunga brown cowabunga\n";
        REQUIRE(out == nullptr);
    }

}

TEST_CASE ("Testing SPACE_USED") {
    Cleaner cleanup;

    Cache::key_type key = "cachekey";

	std::string s1 = "message1";
	Cache::index_type size = 1;
    Cache::val_type val = string_to_val(s1, size);
	std::string s2 = "message2";
    Cache::val_type val2 = string_to_val(s2, size);
    cleanup.add(val, size);
    cleanup.add(val2, size);

    std::shared_ptr<Cache> cacheP = CacheHolder::getCachePointer();

    SECTION ("Space used by an empty cache is 0") {
        REQUIRE(cacheP->space_used() == 0);
    }

    SECTION ("Adding an item increases space used") {
        cacheP->set(key, val, size);
		Cache::index_type spused = cacheP->space_used();
		cacheP->del(key);
        REQUIRE(spused == size);
    }

    SECTION ("Adding and then removing an item does not change space used") {
        cacheP->set(key, val, size);
        cacheP->del(key);
        REQUIRE(cacheP->space_used() == 0);
    }

    SECTION ("Overwriting a new value to the same key does not change space used") {
        cacheP->set(key, val, size);
        cacheP->set(key, val2, size);
        REQUIRE(cacheP->space_used() == size);
    }

}

// Test the functionality of the set function on a cache in a variety
// of cases 
TEST_CASE ("Testing SET functionality") {
    // Create a cleaner object to keep track of pointers and delete them,
    // Create keys and values to be used in population of caches in testing

    Cleaner cleanup;
    //Cache::index_type maxmem = 12;
    //Cache set_test_cache(maxmem);
    //Cache::index_type size = sizeof(int);

	std::shared_ptr<Cache> cacheP = CacheHolder::getCachePointer();

    Cache::key_type key1 = "first_key";
	std::string s1 = "first_message";
	Cache::index_type size1 = 1;
    Cache::val_type val1 = string_to_val(s1, size1);

    Cache::key_type key2 = "second_key";
	std::string s2 = "second_message";
	Cache::index_type size2 = 1;
    Cache::val_type val2 = string_to_val(s2, size2);

    Cache::key_type key3 = "third_key";
    std::string s3 = "third_message";
	Cache::index_type size3 = 1;
    Cache::val_type val3 = string_to_val(s3, size3);

    Cache::key_type key4 = "fourth_key";
    std::string s4 = "fourth_message";
	Cache::index_type size4 = 1;
    Cache::val_type val4 = string_to_val(s4, size4);

    cleanup.add(val1, size1);
    cleanup.add(val2, size2);
    cleanup.add(val3, size3);
    cleanup.add(val4, size4);

    // Set get_size to a pointless int value that will be changed 
    // when a get is run to reflect the size of the value gotten
    Cache::index_type get_size = 1;

    SECTION ("Set a value in an empty cache") {
        
        cacheP->set(key1, val1, size1);

        Cache::val_type get_result = cacheP->get(key1, get_size); 

		if (get_result != nullptr) {cleanup.add(get_result, get_size);}

        // Check that the get method on the key returns a pointer to the expected
        // value associated with that key
		cacheP->del(key1);
        REQUIRE(val_to_string(get_result, get_size) == s1);
    }

    SECTION ("Set a value overwriting an existing val in cache") {
        // Set a value into the cache that can be overwritten
        cacheP->set(key1, val1, size1);
        // Overwrite the first value with a different value on the same key
        cacheP->set(key1, val2, size2);

        Cache::val_type get_result = cacheP->get(key1, get_size);

		if (get_result != nullptr) {cleanup.add(get_result, get_size);}

        // Check that the get method on the initial key returns the 
        // updated value
		cacheP->del(key1);
        REQUIRE( val_to_string(get_result, get_size) == s2 );

    }
	/*
    SECTION ("Try to set a value in a full cache") {
        // filling cache
        set_test_cache.set(key1, val1, size);
        set_test_cache.set(key2, val2, size);
        set_test_cache.set(key3, val3, size);
        // setting to the full cache
        set_test_cache.set(key4, val4, size);

        // check that the new key gets the right val from the cache
        REQUIRE( *((int*) set_test_cache.get(key4, get_size)) == *((int*) val4));
    }
	
    // The setup for this is similar to the previous test, 
    // but the REQUIRE is checking for something different,
    // so it must be done separately.
    SECTION ("Check that the cache does not store more than its maxmem") {
        // Populating cache
        set_test_cache.set(key1, val1, size);
        set_test_cache.set(key2, val2, size);
        set_test_cache.set(key3, val3, size);
        // Setting to the full cache
        set_test_cache.set(key4, val4, size);
        // Checking that used_space after setting more values 
        // than can fit in memory does not exceed maxmem
        Cache::index_type used_space = set_test_cache.space_used();
        REQUIRE( used_space == maxmem );
    }
	*/
    SECTION ("Check that the value is deep copied when it is set") {

        cacheP->set(key1, val1, size1);
        Cache::val_type get_result = cacheP->get(key1, get_size);
		if (get_result != nullptr) {cleanup.add(get_result, get_size);}
		cacheP->del(key1);
        // If the pointer from get_result matches the pointer to 
        // val1, we know val1 wasn't deep copied when it was set to 
        // the cache. 
        REQUIRE( get_result != val1 );
    }
	
}

TEST_CASE ("Testing DEL") {
    Cleaner cleanup;

    Cache::key_type key = "cachekey";
	std::string s = "some_message";
	Cache::index_type size = 1;
    Cache::val_type val = string_to_val(s, size);
    cleanup.add(val, size);

    Cache::index_type get_size = 1;

    std::shared_ptr<Cache> cacheP = CacheHolder::getCachePointer();

    SECTION ("Deleting an item present in the cache removes the item") {
        cacheP->set(key, val, size);
        cacheP->del(key);
        Cache::val_type out = cacheP->get(key, get_size);
        REQUIRE(out == nullptr);
    }

    SECTION ("Deleting an item that isn't in the cache does nothing") {
        cacheP->set(key, val, size);
        cacheP->del("nottherightkey");
        Cache::val_type out = cacheP->get(key, get_size);
		cacheP->del(key);
        REQUIRE(val_to_string(out, get_size) == s);
    }

}
