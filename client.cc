// Client side C/C++ program to demonstrate Socket programming 

#include <iostream>

#include "cache.hh"

#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h> 
#include <string>
#include <cassert>

#include "HTTP_utilities.hh"

const int PORT = 6114;
const std::string SERVER = "127.0.0.1";

struct Cache::Impl {
	// Create a dummy client socket to be set by socket() and an empty buffer for messages 
	int sock;
	struct sockaddr_in address;
	struct sockaddr_in serv_addr;
	char buffer[MAX_MESSAGE_SIZE] = {0};

	Impl(Cache::index_type maxmem, Cache::hash_func hasher) {
		if (maxmem == maxmem + 1) {
			if (hasher) {
				sock = 1;
			}
		}
	
		sock = 0;

		// Creates a socket to assign to 'sock' or gives an error message if creation fails
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("\n Socket creation error \n"); 
			assert(false);
		}

		memset(&serv_addr, '0', sizeof(serv_addr)); 
		serv_addr.sin_family = AF_INET; 
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(PORT); 

		// Converts server address into binary form or prints a message if invalid address
		if(inet_pton(AF_INET, SERVER.c_str(), &serv_addr.sin_addr)<=0) { 
			printf("\nInvalid address/ Address not supported \n"); 
			assert(false); 
		} 

		// Connects to the server or prints a message if unsuccessful
		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
			printf("\nConnection Failed \n"); 
			assert(false); 
		} 

		recv(sock, buffer, MAX_MESSAGE_SIZE, 0);
		std::cout<<buffer<<"\n";
	}

	~Impl() {
	}

	/* Handles get requests, formatting and passing them to the server, receiving and parsing
	   the server's response, and returning a void pointer to the value retrieved, or nullptr
	   if the get is unsuccessful. */ 
	Cache::val_type get(Cache::key_type key, Cache::index_type &valsize){
		HTTP_request request;
		request.verb = "GET";
		request.URI = "/key/" + key;

		const char* request_string = (request.to_string()).c_str();
		send(sock, request_string, request.to_string().length(), 0);
		memset(buffer, '\0', MAX_MESSAGE_SIZE);
		if ((recv(sock, buffer, MAX_MESSAGE_SIZE, 0) < 0) || (strlen(buffer) == 0)) {
			std::cout<<"'GET' failed - empty buffer - trying to get '" << key << "'\n";
			valsize = 0;
			return nullptr;
		}
		HTTP_response response;
		response.parse_raw_response(buffer);
		if (response.code == "200") {
			JSON result;
			result.parse_string(response.body);
			auto r = *(result.find(key));
			Cache::val_type val = string_to_val((result.find(key))->second, valsize);
			return val;
		} else {
			valsize = 0;
			return nullptr;
		}
	}

	/* Handles client set requests, requesting, parsing server response, and returning
	a value indicating success or failure based on the code returned by the server. */
	int set(Cache::key_type key, Cache::val_type val, Cache::index_type size){
		// Create an empty HTTP request, fill it with the indicated put request 
		HTTP_request request;
		request.verb = "PUT";
		request.URI = "/key/" + key + "/" + val_to_string(val, size);
		// Convert the request to a c-string so it can be passed to the socket
		const char* request_string = (request.to_string()).c_str();
		send(sock, request_string, request.to_string().length(), 0);
		// Zero the buffer in preparation for receipt of the server reponse 
		memset(buffer, 0, MAX_MESSAGE_SIZE);
		if ((recv(sock, buffer, MAX_MESSAGE_SIZE, 0) < 0) || (strlen(buffer) == 0)) {
			return -2;
		}
		HTTP_response response;
		response.parse_raw_response(buffer);
		if (response.code == "200") {
			return 0;
		} else {
			return -1;
		}
	}

	/* Handles client requests to delete, parses server response and returns a value indicating 
	success or failure based on the response code returned by the server. */
	int del(Cache::key_type key){
		HTTP_request request;
		request.verb = "DELETE";
		request.URI = "/key/" + key;
		//std::cout<<"requet URI is: " + request.URI + "\n";
		//std::cout<<request.to_string();
		auto a = request.to_string();
		const char* request_string = a.c_str();
		//std::cout<<"I'm the client, I'm deleting, and my request is: " << request_string<<"\n";
		//std::cout<<"length of the message I'm sending: " << request.to_string().length() << "\n";
		//std::cout<<"altlen: " << strlen(request_string);
		send(sock, request_string, request.to_string().length(), 0);
		memset(buffer, '\0', MAX_MESSAGE_SIZE);
		//recv(sock, buffer, MAX_MESSAGE_SIZE, 0);
		if ((recv(sock, buffer, MAX_MESSAGE_SIZE, 0) < 0) || (strlen(buffer) == 0)) {
			return -2;
		}
		HTTP_response response;
		response.parse_raw_response(buffer);
		if (response.code == "200") {
			return 0;
		} else {
			return -1;
		}
	}
	
	/* Handles the client side of calls to space_used, from sending request to parsing 
	   the server response, and returns a void pointer to the value of current space used 
	   in the cache. */
	Cache::index_type space_used() {
		// Create an HTTP request set to GET/memsize
		HTTP_request request;
		request.verb = "GET";
		request.URI = "/memsize";
		// Convert the HTTP request to a c-style string so it can be passed to the socket
		const char* request_string = (request.to_string()).c_str();
		// Send the request over the socket
		send(sock, request_string, request.to_string().length(), 0);
		// Make sure the buffer is clear before receiving response from the server
		memset(buffer, '\0', MAX_MESSAGE_SIZE);
		recv(sock, buffer, MAX_MESSAGE_SIZE, 0);
		// Create an empty HTTP response to parse the server response into
		HTTP_response response;
		response.parse_raw_response(buffer);
		// Create an empty JSON and parse the server's response into it
		JSON result;
		result.parse_string(response.body);
		// Find the returned value for memory used in the k-v pair it belongs to and return it
		std::string size_string = result.find("memused")->second;
		unsigned to_return = (unsigned) stoi(size_string);
		return to_return;
	}

};

// Constructs the cache and consequently the Impl
Cache::Cache(Cache::index_type maxmem, hash_func hasher) 
	: pImpl_(new Impl(maxmem, hasher))
{
}

Cache::~Cache(){
}

// Calls set in Impl
int Cache::set(Cache::key_type key, Cache::val_type val, Cache::index_type size){
	return pImpl_->set(key, val, size);
}

// Calls get in Impl
Cache::val_type Cache::get(Cache::key_type key, Cache::index_type& size) const{
	return pImpl_->get(key, size);
}

// Calls del in Impl
int Cache::del(Cache::key_type key){
	return pImpl_->del(key);
}

// Calls space_used in Impl
Cache::index_type Cache::space_used() const{
	return pImpl_->space_used();
}