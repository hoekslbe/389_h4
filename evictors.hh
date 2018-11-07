#include "cache.hh"
#include <list>
#include <unordered_map>
#include <cstring>

//An evictor implementing a FIFO replacement policy.  
struct FIFO_Evictor  {
	FIFO_Evictor();
	~FIFO_Evictor();

	//Called by a cache whenever an item is added.  
	void added_item (Cache::key_type key);
	
	//Called by a cache whenever an item is accessed.  In FIFO_Evictor, 
	// this does nothing.  
	void accessed_item (Cache::key_type key);

	//Called by a cache whenever an item is removed (by the 
	// user or through eviction)
	void deleted_item (Cache::key_type key);

	//Called by a cache to get the key of the next item to evict.  
	Cache::key_type get_evict_target ();
	
	//A list containing all the keys; it's maintained as a priority queue indicating which item
	// to evict next.  
	std::list<std::string> _key_list;
	//An unordered map which maps keys to the element of _key_list containing them,
	// allowing for fast access.  
	std::unordered_map<std::string, std::list<std::string>::iterator> _key_map;
};

//An evictor implementing an LRU replacement policy.  
// This works pretty much exactly like FIFO_Evictor, but it actually does something when 
// an item is accessed.  
struct LRU_Evictor {
	LRU_Evictor();
	~LRU_Evictor();

	void added_item (Cache::key_type key);
	void accessed_item (Cache::key_type key);
	void deleted_item (Cache::key_type key);
	Cache::key_type get_evict_target ();

	std::list<std::string> _key_list;
	std::unordered_map<std::string, std::list<std::string>::iterator> _key_map;
};