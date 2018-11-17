#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <vector>
#include <utility>

#include "cache.hh"

#include <iostream>

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


TEST_CASE ("Testing GET"){
    // A cleaner is always created to handle the pointers needed for a test
    Cleaner cleanup;

    // Create keys and values that can be used to populate the cache
    // And create the cache itself before running tests
    Cache::key_type key = "cachekey";
    Cache::val_type val = (void*) new int(3);
    Cache::index_type size = sizeof(int);
    cleanup.add(val, size);

    Cache::val_type val2 = (void*) new double(4.0);
    Cache::index_type size2 = sizeof(double);
    cleanup.add(val2, size2);

    Cache::index_type get_size = 1;

    Cache cache(size + size2 + 1);
    SECTION ("Getting an item present in the cache returns the item") {
		std::cout<<"hi\n";
        cache.set(key, val, size);
		std::cout<<"whi\n";
        Cache::val_type out = cache.get(key, get_size);
		std::cout<<"bye\n";
		std::cout<<out<<"\n";
		if (out != nullptr) {cleanup.add(out, get_size);}
		std::cout<<"wumba\n";
        REQUIRE(*((int*) out) == *((int*) val));
    }

    SECTION ("Get sets valsize to the size of the returned object") {
		std::cout<<"mumba\n";
        cache.set(key, val, size);
		std::cout<<"mumbo\n";
        auto val = cache.get(key, get_size);
		std::cout<<"jumbo\n";
		if (val != nullptr) {cleanup.add(val, get_size);}
        REQUIRE(get_size == size);
    }

    SECTION ("Get doesn't have a hard-coded val_size to return") {
        cache.set(key, val2, size2);
        auto val = cache.get(key, get_size);
		if (val != nullptr) {cleanup.add(val, get_size);}
        REQUIRE(get_size == size2);
    }

    SECTION ("Getting an item not in the cache returns nullptr") {
        // calls get without ever setting
        Cache::val_type out = cache.get(key, get_size);
		if (out != nullptr) {cleanup.add(out, get_size);}
        REQUIRE(out == nullptr);
    }

}

TEST_CASE ("Testing SPACE_USED") {
    Cleaner cleanup;

    Cache::key_type key = "cachekey";
    Cache::val_type val = (void*) new int(3);
    Cache::val_type val1 = (void*) new int(2);
    Cache::index_type size = sizeof(int);
    cleanup.add(val, size);
    cleanup.add(val1, size);

    Cache cache(size + 1);

    SECTION ("Space used by an empty cache is 0") {
        REQUIRE(cache.space_used() == 0);
    }

    SECTION ("Adding an item increases space used") {
        cache.set(key, val, size);
        REQUIRE(cache.space_used() == size);
    }

    SECTION ("Adding and then removing an item does not change space used") {
        cache.set(key, val, size);
        cache.del(key);
        REQUIRE(cache.space_used() == 0);
    }

    SECTION ("Overwriting a new value to the same key does not change space used") {
        cache.set(key, val, size);
        cache.set(key, val1, size);
        REQUIRE(cache.space_used() == 4);
    }

}

// Test the functionality of the set function on a cache in a variety
// of cases 
TEST_CASE ("Testing SET functionality") {
    // Create a cleaner object to keep track of pointers and delete them,
    // Create keys and values to be used in population of caches in testing

    Cleaner cleanup;
    Cache::index_type maxmem = 12;
    Cache set_test_cache(maxmem);
    Cache::index_type size = sizeof(int);

    Cache::key_type key1 = "first_key";
    Cache::val_type val1 = (void*) new int(1);

    Cache::key_type key2 = "second_key";
    Cache::val_type val2 = (void*) new int(2);

    Cache::key_type key3 = "third_key";
    Cache::val_type val3 = (void*) new int(3);

    Cache::key_type key4 = "fourth_key";
    Cache::val_type val4 = (void*) new int(4);

    cleanup.add(val1, size);
    cleanup.add(val2, size);
    cleanup.add(val3, size);
    cleanup.add(val4, size);

    // Set get_size to a pointless int value that will be changed 
    // when a get is run to reflect the size of the value gotten
    Cache::index_type get_size = 1;

    SECTION ("Set a value in an empty cache") {
        
        set_test_cache.set(key1, val1, size);

        Cache::val_type get_result = set_test_cache.get(key1, get_size); 

		if (get_result != nullptr) {cleanup.add(get_result, get_size);}

        // Check that the get method on the key returns a pointer to the expected
        // value associated with that key
        REQUIRE( *((int*) get_result) == *((int*) val1));
    }

    SECTION ("Set a value overwriting an existing val in cache") {
        // Set a value into the cache that can be overwritten
        set_test_cache.set(key1, val1, size);
        Cache::index_type get_size = sizeof(int);
        // Overwrite the first value with a different value on the same key
        set_test_cache.set(key1, val2, size);

        Cache::val_type get_result = set_test_cache.get(key1, get_size);

		if (get_result != nullptr) {cleanup.add(get_result, get_size);}

        // Check that the get method on the initial key returns the 
        // updated value
        REQUIRE( *((int*) get_result) == *((int*) val2));

    }

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

    SECTION ("Check that the value is deep copied when it is set") {

        set_test_cache.set(key1, val1, size);
        Cache::val_type get_result = set_test_cache.get(key1, get_size);

        // If the pointer from get_result matches the pointer to 
        // val1, we know val1 wasn't deep copied when it was set to 
        // the cache. 
        REQUIRE( get_result != val1 );
    }
	
}

TEST_CASE ("Testing DEL") {
    Cleaner cleanup;

    Cache::key_type key = "cachekey";
    Cache::val_type val = (void*) new int(3);
    Cache::index_type size = sizeof(int);
    cleanup.add(val, size);

    Cache::index_type get_size = 1;

    Cache cache(size + 1);

    SECTION ("Deleting an item present in the cache removes the item") {
        cache.set(key, val, size);
        cache.del(key);
        Cache::val_type out = cache.get(key, get_size);
        REQUIRE(out == nullptr);
    }

    SECTION ("Deleting an item that isn't in the cache does nothing") {
        cache.set(key, val, size);
        cache.del("nottherightkey");
        Cache::val_type out = cache.get(key, get_size);
        REQUIRE(*((int*) out) == *((int*) val));
    }

}