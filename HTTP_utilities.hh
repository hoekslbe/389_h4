#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

#pragma once

//Need to change this to our actual HTTP version (whatever it is).  
const std::string HTTP_VERSION = "HTTP v1.1";
const unsigned MAX_MESSAGE_SIZE = 4096;


// Takes in a void pointer and an unsigned int, and creates a string out of the first *size* bytes at the void pointer.  
std::string val_to_string(const void* val, const unsigned size);

// Takes in a string (s) and an unsigned int reference (size).  Returns a void pointer pointing to a copy of the bytes of s, and sets size to the number of bytes pointed to by the void pointer.  
// Note: allocates memory.  
void* string_to_val(std::string s, unsigned &size);

// Struct encompassing HTTP_request formatting operations
struct HTTP_request {
	HTTP_request();
	~HTTP_request();

	//Takes in raw, a c-style string, and uses the data in it to fill out the HTTP_request data fields.
	void parse_raw_request (char* raw);
	// Converts the data within an HTTP_request object to a std::string and returns it
	std::string to_string();
	// HTTP_request data fields 
	std::string verb;
	std::string URI;
	std::string version;
	std::vector<std::string> header_lines;
	std::string body;
};

// Struct encompassing HTTP_response formatting operations
struct HTTP_response {

	HTTP_response();
	~HTTP_response();
	// Takes in raw, a c-style string, and parses it into an 
	// HTTP response object with fields properly assigned according to 
	// the data within the string.
	void parse_raw_response(char* raw);
	// Converts an HTTP_response object into a std::string and returns it
	std::string to_string();
	// HTTP response data fields
	std::string version;
	std::string code;
	std::vector<std::string> header_lines;
	std::string body;
};

// Struct encompassing JSON formatted string operations
struct JSON {

	JSON ();
	~JSON ();
	// Parses a std::string into JSON format 
	void parse_string(std::string);
	// Converts a JSON object into std::string format and returns the string 
	std::string to_string();
	// Adds a key-val pair to the unordered key-value map when the val is not of 
	// std::string form yet
	void add (const std::string key, const void* val, const unsigned size);
	// Adds a std::string key value pair to the unordered key-value map
	void add (const std::string key, const std::string val);
	// Find a value in the key-value map by key string
	std::unordered_map<std::string, std::string>::iterator find(std::string key);
	std::unordered_map<std::string, std::string> kv_map_;
};
