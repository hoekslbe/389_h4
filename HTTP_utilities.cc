#include "HTTP_utilities.hh"



struct HTTP_request {
	HTTP_request() {
		verb = "";
		URI = "";
		version = HTTP_VERSION;
		header = "";
		body = "";
	}

	~HTTP_request() {}

	void parse_request (char* raw) {

	
	}

	const char* to_cstring() {
		return (verb_ + " " + URI_ + " " + version_ + "\n" 
			+ header_ + "\n" 
			+ body_).c_string();
	}

	std::string verb;
	std::string URI;
	std::string version;
	std::string header;
	std::string body;
};

struct HTTP_response {
	HTTP_response() {}
	~HTTP_response(){}

	void parse_response(const char* raw) {
		
	}

	const char* to_cstring() {
		return (version_ + " " + code_ + "\n"
			+ header_ + "\n"
			+ body_).c_string();
	}

	std::string version;
	std::string code;
	std::string header;
	std::string body;
};

struct JSON {
	JSON () {}
	~JON () {}
	void parse_string(std::string) {}
	std::string to_string(){}
	void add (std::string key, void* value, unsigned size) {}
	std::unordered_map<std::string, char*> kv_map_;
};

std::unordered_map<std::string, char*> parse_JSON (char* obj) { // remember: this is just going to be a bunch of pointers into the buffer.  
	std::unordered_map <char*, char*> kv_pair_map;
	const char* delimiters = "{ ,}";
	std::string k = "";
	const char* token = strtok (obj, delimiters);
	while (token != nullptr) {
		if (k == "") {
			k = token; // will this conversion work?  
		} else {
			kv_pair_map[k] = token; // right syntax?  
			k = "";
		}
		token = strtok (nullptr, delimiters);
	}
	return kv_pair_map;
}