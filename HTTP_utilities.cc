#include "HTTP_utilities.hh"
#include <iostream>

// Accesses the void* val and converts 'size' number of bytes from that 
// pointer to a std::string representation of the data stored at the pointer.  
std::string val_to_string(const void* val, const unsigned size) {
	char interim_conversion_string[MAX_MESSAGE_SIZE];
	memset(interim_conversion_string, '\0', size + 1);
	memcpy((void*) interim_conversion_string, val, size);
	std::string converted_string(interim_conversion_string);
	return converted_string;
}

// Takes a std::string and a dummy reference 'size' and returns a void pointer
// allocated to the c-string representation of the input std::string.
void* string_to_val (std::string s, unsigned &size) {
	const char* interim_conversion_string = s.c_str();
	size = strlen(interim_conversion_string);
	// Allocates 'size' number of bytes to a void pointer now that 'size'
	// is set to the length of the input string's c-string form
	void* val = operator new(size);
	memcpy(val, (void*) interim_conversion_string, size);
	return val;
}

// HTTP request conversion tools 

// Constructor and destructor for HTTP requests 
HTTP_request::HTTP_request() {
	verb = " ";
	URI = " ";
	version = HTTP_VERSION;
	body = " ";
}
HTTP_request::~HTTP_request() {}

// Takes in a char pointer to an unformatted 
// HTTP_request string and parses it into tokens in order
// to assign the proper values to the HTTP_request fields based on input
void HTTP_request::parse_raw_request (char* raw) {
	const char* delimiter = "\n";
	char* token;
	token = strtok(raw, delimiter);
	verb = token;
	token = strtok(nullptr, delimiter);
	URI = token;
	token = strtok(nullptr, delimiter);
	version = token;
	token = strtok(nullptr, delimiter);
	while ((token[0] != ' ') && (token[0] != '\0')) {
		std::string s(token);
		header_lines.push_back(s);
		token = strtok(nullptr, delimiter);
	}
	token = strtok(nullptr, delimiter);
	body = token;
}

// Converts an HTTP_request object to a std::string 
std::string HTTP_request::to_string() {
	std::string to_return = "";
	to_return += verb + "\n";
	to_return += URI + "\n";
	to_return += version + "\n";
	for (auto h : header_lines) {
		to_return += h + "\n";
	}
	to_return += " \n";
	to_return += body + "\n";
	return to_return;
}

// HTTP response conversion tools 

// HTTP response constructor and destructor
HTTP_response::HTTP_response() {
	version = HTTP_VERSION;
	code = " ";
	body = " ";
}
HTTP_response::~HTTP_response(){}

// Parses a raw HTTP response string taken in as a char pointer to the 
// string, 
void HTTP_response::parse_raw_response (char* raw) {
	const char* delimiter = "\n";
	char* token;
	token = strtok(raw, delimiter);
	version = token;
	token = strtok(nullptr, delimiter);
	code = token;
	token = strtok(nullptr, delimiter);
	while ((token[0] != ' ') && (token[0] != '\0')) {
		std::string s(token);
		header_lines.push_back(s);
		token = strtok(nullptr, delimiter);
	}
	token = strtok(nullptr, delimiter);
	body = token;
}

// Converts an HTTP response to a std::string and returns the string
std::string HTTP_response::to_string() {
	std::string to_return = "";
	to_return += version + "\n";
	to_return += code + "\n";
	for (auto h : header_lines) {
		to_return += h + "\n";
	}
	to_return += " \n";
	to_return += body + "\n";
	return to_return;
}

// JSON conversion tools

// JSON formatted string constructor and destructor 
JSON::JSON () {}
JSON::~JSON () {}

// Converts a properly formatted JSON string into an unordered map owned by the object
void JSON::parse_string(std::string s) {
	char * to_parse = (char*)(s.c_str());
	const char* delimiters = "{ ,:}";
	std::string k = "";
	char* token = strtok (to_parse, delimiters);
	while (token != nullptr){
		if (k == ""){
			k = token;
		} else {
			kv_map_[k] = static_cast<std::string>(token);
			k = "";
		}
		token = strtok (nullptr, delimiters);
	}
}

// Finds a key by string in the JSON object map 
std::unordered_map<std::string, std::string>::iterator JSON::find(std::string key) {
	return kv_map_.find(key);
}

// Converts a JSON object's unordered map into a JSON-formatted string
std::string JSON::to_string(){
	std::string to_return = "{ ";
	auto kv_it = kv_map_.begin();
	if (kv_it != kv_map_.end()) {
		to_return += kv_it->first + ": " + kv_it->second;
	}
	kv_it++;
	while (kv_it != kv_map_.end()) {
		to_return += ", "+ kv_it->first + ":"+ kv_it->second;
		kv_it++;
	}
	to_return = to_return + " }";
	return to_return;
}

// Takes a key in std::string form, a void pointer to some value, 
// and a size for that value, passes the val and size to the val_to_string
// conversion function to format it, and maps the key and value into the JSON object's k-v map. 
void JSON::add(const std::string key, const void* val, const unsigned size) {
	kv_map_[key] = val_to_string(val, size);
}

// Takes a key and value in std::string form and puts them in the JSON object's k-v map.
void JSON::add(const std::string key, const std::string val) {
	kv_map_[key] = val;
}