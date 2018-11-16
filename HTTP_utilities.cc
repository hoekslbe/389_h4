#include "HTTP_utilities.hh"


std::string val_to_string(const void* val, const unsigned size) {
	char interim_conversion_string[size + 1];
	memset(interim_conversion_string, '\0', size + 1);
	memcpy((void*) interim_conversion_string, val, size);
	std::string converted_string(interim_conversion_string);
	return converted_string;
}

void* string_to_val (std::string s, unsigned &size) {
	char* interim_conversion_string = s.c_str();
	size = strlen(interim_conversion_string);
	void* val = operator new(size);
	memcpy(val, (void*) interim_conversion_string, size);
	return val;
}

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