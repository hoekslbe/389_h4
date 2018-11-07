#include "evictors.hh"


	FIFO_Evictor::FIFO_Evictor () {}
	FIFO_Evictor::~FIFO_Evictor () {}

	//keeps track of an item added to the cache
	void FIFO_Evictor::added_item (Cache::key_type key) {
		std::string key_copy = key;
		_key_list.push_front(key_copy);
		_key_map[key] = _key_list.begin();
	}

	// Does nothing
	void FIFO_Evictor::accessed_item (Cache::key_type key) {
		// doing nothing with key to avoid a compiler warning
		Cache::key_type _ = key; 
	}

	//gets rid of the key for an item that was removed from cache.  
	void FIFO_Evictor::deleted_item (Cache::key_type key) {
		auto found_at = _key_map.find(key);
		if (found_at != _key_map.end()) {
			_key_list.erase(found_at->second);
			_key_map.erase(found_at);
		}
	}

	//returns the key of the next item to evict from cache.  
	Cache::key_type FIFO_Evictor::get_evict_target () { 
		return (Cache::key_type) _key_list.back();
	}



	LRU_Evictor::LRU_Evictor () {}
	LRU_Evictor::~LRU_Evictor () {}

	//Does the same thing as FIFO_evictor's version.  
	void LRU_Evictor::added_item (Cache::key_type key) {
		std::string key_copy = key;
		_key_list.push_front(key_copy);
		_key_map[key] = _key_list.begin();
	}

	//bumps an item to the front of _key_list so that it's now the last item to be evicted
	// (since it's the most recently accessed).  
	void LRU_Evictor::accessed_item (Cache::key_type key) {
		deleted_item(key);
		added_item(key);
	}

	//Does the same thing as FIFO_evictor's version.  
	void LRU_Evictor::deleted_item (Cache::key_type key) {
		auto found_at = _key_map.find(key);
		if (found_at != _key_map.end()) {
			_key_list.erase(found_at->second);
			_key_map.erase(found_at);
		}
	}

	//Does the same thing as FIFO_evictor's version.  
	Cache::key_type LRU_Evictor::get_evict_target () { 
		return (Cache::key_type) _key_list.back();
	}