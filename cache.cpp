
#include "evictors.hh"
#include <unordered_map>
#include <memory>
 

//The hidden implementation for a cache
struct Cache::Impl {

		Impl(Cache::index_type maxmem, Cache::hash_func hasher)
			: _cache_map(0, hasher)
		{
			// this does nothing - just avoiding a compiler warning for ignoring evictor.  
			
			_maxmem = maxmem;
			_usedmem = 0;
		};
		~Impl(){
			while (_usedmem > 0) {
				evict();
			}
		};
		/* look for key in cache, if something exists with key, overwrite it to new value, otherwise 
		if there's space for new key-value pair, add value to the cache with key. 
		if there is no space, evict items from the cache until there is enough space.   
		if the item being added is too big to fit in the cache, do nothing.  
		*/
		int set(Cache::key_type key, Cache::val_type val, Cache::index_type size){
			if (size <= _maxmem) {
				auto key_copy = key;
				auto size_copy = size;

				void* val_copy = operator new(size_copy);
				std::memcpy(val_copy, val, size);
				std::pair<void*, Cache::index_type>* to_insert = new std::pair<void*, Cache::index_type> (val_copy, size_copy);

				//if the item being added is already in the cache, get rid of it.  
				auto found_at = _cache_map.find(key);
				if (found_at != _cache_map.end()) {
					del(found_at->first);
				}

				//evict items from the cache until there is room to add the item being added.  
				while (space_used() + size_copy > _maxmem) {
					evict();
				}
			
				_evictor.added_item(key_copy);
				_usedmem += size_copy;
				_cache_map[key_copy] = (void*) to_insert;
				return 0;
			}
			return -1;
		}

		// retrieve a pointer to the value associated with key in cache,
		// or NULL if not found.
		// sets actual size of the returned value (in bytes) in val_size
		Cache::val_type get(Cache::key_type key, Cache::index_type& size){
			auto found_at = _cache_map.find(key);
			if (found_at != _cache_map.end()) {
				_evictor.accessed_item(key);
				auto retrieved = (std::pair<Cache::val_type, Cache::index_type>*) found_at->second;
				size = retrieved->second;
				return retrieved->first;
			} else {
				size = 0;
				return nullptr;
			}

		}

		// delete an object from the cache if it is there 
		int del(Cache::key_type key){
			auto found_at = _cache_map.find(key);
			if (found_at != _cache_map.end()) {
				auto being_removed = (std::pair<void*, Cache::index_type>*) found_at->second;
				_usedmem -= being_removed->second;
				operator delete(being_removed->first, being_removed->second);
				delete being_removed;
				_evictor.deleted_item(key);
				_cache_map.erase(found_at);
			}
			return 0;
		}
		// compute the total amount of memory used up by all cache values (not keys)
		Cache::index_type space_used(){
			return _usedmem;
		}

		// get rid of some cache item to free up space
		void evict () {
			Cache::key_type to_evict = _evictor.get_evict_target();
			del(to_evict);
		}

		//We interpret the void* below as a std::pair of < Cache::val_type, Cache::index_type>
		std::unordered_map<std::string, void*, Cache::hash_func> _cache_map;
		Cache::index_type _maxmem;
		Cache::index_type _usedmem;
		LRU_Evictor _evictor;
};

// construct the cache and consequently the Impl
Cache::Cache(Cache::index_type maxmem, hash_func hasher) 
	: pImpl_(new Impl(maxmem, hasher))
{
}

Cache::~Cache(){
}

// Call set in Impl
int Cache::set(Cache::key_type key, Cache::val_type val, Cache::index_type size){
	return pImpl_->set(key, val, size);
}

// Call get in Impl
Cache::val_type Cache::get(Cache::key_type key, Cache::index_type& size) const{
	return pImpl_->get(key, size);
}

// Call del in Impl
int Cache::del(Cache::key_type key){
	return pImpl_->del(key);
}

// Call space_used in Impl
Cache::index_type Cache::space_used() const{
	return pImpl_->space_used();
}


