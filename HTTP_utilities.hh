#include <cstring>
#include <string>
#include <unordered_map>

const std::string HTTP_VERSION = "?";

struct HTTP_request {
	HTTP_request();

	~HTTP_request();

	void parse_request (char* raw);

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

	void parse_response(const char* raw);

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
