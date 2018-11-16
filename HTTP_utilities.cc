#include "HTTP_utilities.hh"


std::string val_to_string(const void* val, const unsigned size) {
	char interim_conversion_string[1024];
	memset(interim_conversion_string, '\0', size + 1);
	memcpy((void*) interim_conversion_string, val, size);
	std::string converted_string(interim_conversion_string);
	return converted_string;
}

void* string_to_val (std::string s, unsigned &size) {
	const char* interim_conversion_string = s.c_str();
	size = strlen(interim_conversion_string);
	void* val = operator new(size);
	memcpy(val, (void*) interim_conversion_string, size);
	return val;
}


HTTP_request::HTTP_request() {
	verb = "";
	URI = "";
	version = HTTP_VERSION;
	header = "";
	body = "";
}

HTTP_request::~HTTP_request() {}

void HTTP_request::parse_raw_request (char* raw) {
	const char* newline_delimiter = "\n";
	const char* space_delimiter = " ";
	verb = strtok(raw, space_delimiter);
	URI = strtok(nullptr, space_delimiter);
	version = strtok(nullptr, newline_delimiter);
	header = strtok(nullptr, newline_delimiter);
	body = strtok(nullptr, newline_delimiter);	
}

const char* HTTP_request::to_cstring() {
	return (verb + " " + URI + " " + version + "\n" 
		+ header + "\n" 
		+ body + "\n").c_str();
}


HTTP_response::HTTP_response() {
	version = HTTP_VERSION;
	code = "";
	header = "";
	body = "";
}
HTTP_response::~HTTP_response(){}

void HTTP_response::parse_raw_response (char* raw) {
	const char* newline_delimiter = "\n";
	const char* space_delimiter = " ";
	version = strtok(raw, space_delimiter);
	code = strtok(nullptr, newline_delimiter);
	header = strtok(nullptr, newline_delimiter);
	body = strtok(nullptr, newline_delimiter);	
}

const char* HTTP_response::to_cstring() {
	return (version + " " + code + "\n"
		+ header + "\n"
		+ body + "\n").c_str();
}


JSON::JSON () {}
JSON::~JSON () {}
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
	}
}

std::unordered_map<std::string, std::string>::iterator JSON::find(std::string key) {
	return kv_map_.find(key);
}
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

void JSON::add(std::string key, void* val, unsigned size) {
	kv_map_[key] = val_to_string(val, size);
}

