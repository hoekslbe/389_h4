#include "HTTP_utilities.hh"
#include <iostream>

std::string val_to_string(const void* val, const unsigned size) {
	char interim_conversion_string[MAX_MESSAGE_SIZE];
	memset(interim_conversion_string, '\0', size + 1);
	memcpy((void*) interim_conversion_string, val, size);
	std::string converted_string(interim_conversion_string);
	return converted_string;
}

void* string_to_val (std::string s, unsigned &size) {
	const char* interim_conversion_string = s.c_str();
	//size = s.length();
	size = strlen(interim_conversion_string);
	std::cout<<"length was: " << size << '\n';
	void* val = operator new(size);
	memcpy(val, (void*) interim_conversion_string, size);
	return val;
}


HTTP_request::HTTP_request() {
	verb = " ";
	URI = " ";
	version = HTTP_VERSION;
	body = " ";
}

HTTP_request::~HTTP_request() {}

void HTTP_request::parse_raw_request (char* raw) {
	printf("%s\n",raw);
	std::cout<<"1 request\n";
	const char* delimiter = "\n";
	char* token;
	std::cout<<"1.5\n";
	token = strtok(raw, delimiter);
	verb = token;
	std::cout<<"2\n";
	token = strtok(nullptr, delimiter);
	URI = token;
	std::cout<<"3\n";
	token = strtok(nullptr, delimiter);
	version = token;
	std::cout<<"4\n";
	token = strtok(nullptr, delimiter);
	while ((token[0] != ' ') && (token[0] != '\0')) {
		std::cout<<"5\n";
		std::string s(token);
		header_lines.push_back(s);
		token = strtok(nullptr, delimiter);
	}
	std::cout<<"6\n";
	token = strtok(nullptr, delimiter);
	body = token;
	std::cout<<"7\n";
}

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


HTTP_response::HTTP_response() {
	version = HTTP_VERSION;
	code = " ";
	body = " ";
}
HTTP_response::~HTTP_response(){}

void HTTP_response::parse_raw_response (char* raw) {
	printf("%s\n",raw);
	std::cout<<"1 response\n";
	const char* delimiter = "\n";
	char* token;
	std::cout<<"1.5\n"; 
	token = strtok(raw, delimiter);
	version = token;
	std::cout<<"2\n";
	token = strtok(nullptr, delimiter);
	code = token;
	std::cout<<"3\n";
	token = strtok(nullptr, delimiter);
	std::cout<<"4\n";
	while ((token[0] != ' ') && (token[0] != '\0')) {
		std::cout<<"5\n";
		std::string s(token);
		header_lines.push_back(s);
		token = strtok(nullptr, delimiter);
	}
	std::cout<<"6\n";
	token = strtok(nullptr, delimiter);
	body = token;
	std::cout<<"7\n";
}

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
		token = strtok (nullptr, delimiters);
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

void JSON::add(const std::string key, const void* val, const unsigned size) {
	kv_map_[key] = val_to_string(val, size);
}

void JSON::add(const std::string key, const std::string val) {
	kv_map_[key] = val;
}