#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>

//Need to change this to our actual HTTP version (whatever it is).  
const std::string HTTP_VERSION = "?";


// Takes in a void pointer and an unsigned int, and creates a string out of the first *size* bytes at the void pointer.  
std::string val_to_string(const void* val, const unsigned size);

// Takes in a string, and returns a val/size pair where val is a void pointer pointing to a copy of the bytes of the string, and size is the number of bytes pointed to by val.  
// Note: allocates memory (val).  
std::pair<void*, unsigned> string_to_val(std::string s);

struct HTTP_request {
	HTTP_request();

	~HTTP_request();

	//Takes in raw, a c-style string, and uses the data in it to fill out the various fields of the HTTP_request
	void parse_raw (char* raw);

	const char* to_cstring();

	std::string verb;
	std::string URI;
	std::string version;
	std::string header;
	std::string body;
};

struct HTTP_response {
	HTTP_response();
	~HTTP_response();

	//Takes in raw, a c-style string, and uses the data in it to fill out the various fields of the HTTP_request
	void parse_raw(const char* raw);

	const char* to_cstring();

	std::string version;
	std::string code;
	std::string header;
	std::string body;
};

struct JSON {
	JSON ();
	~JON () {}
	void parse_string(std::string);
	std::string to_string();
	void add (std::string key, void* value, unsigned size);
	std::unordered_map<std::string, char*> kv_map_;
};
