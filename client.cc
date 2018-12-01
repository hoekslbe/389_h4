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



//To do:
	//write an http generating function
	//write an http parsing function (a la JSON-parsing function)
	// use them in the Impl methods.  

// Helper function: converts a JSON into an unordered map which sends json-keys to json-values (values are char pointers pointing into subsections of obj, which is changed - keys are c++ strings)

struct Cache::Impl {
	int sock;
	struct sockaddr_in address;
	struct sockaddr_in serv_addr;
	char buffer[MAX_MESSAGE_SIZE] = {0};

	Impl(Cache::index_type maxmem, Cache::hash_func hasher) {
		if (maxmem == maxmem + 1) {
			if (hasher) {
				sock = 1; // do nothing?
			}
		}
	
		sock = 0;

		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("\n Socket creation error \n"); 
			assert(false);
		}

		memset(&serv_addr, '0', sizeof(serv_addr)); 
		serv_addr.sin_family = AF_INET; 
		serv_addr.sin_addr.s_addr = INADDR_ANY;
		serv_addr.sin_port = htons(PORT); 

		

		if(inet_pton(AF_INET, SERVER.c_str(), &serv_addr.sin_addr)<=0) { 
			printf("\nInvalid address/ Address not supported \n"); 
			assert(false); 
		} 
		

		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
			printf("\nConnection Failed \n"); 
			assert(false); 
		} 

		recv(sock, buffer, MAX_MESSAGE_SIZE, 0);
		std::cout<<buffer<<"\n";
	}

	~Impl() {
	}

	Cache::val_type get(Cache::key_type key, Cache::index_type &valsize){
		HTTP_request request;
		request.verb = "GET";
		request.URI = "/key/" + key;

		const char* request_string = (request.to_string()).c_str();
		send(sock, request_string, request.to_string().length(), 0);
		memset(buffer, '\0', MAX_MESSAGE_SIZE);
		recv(sock, buffer, MAX_MESSAGE_SIZE, 0);
		HTTP_response response;
		response.parse_raw_response(buffer);
		if (response.code == "200") {
			JSON result;
			result.parse_string(response.body);
			std::cout<<result.to_string()<<'\n';
			auto r = *(result.find(key));
			std::cout<<r.first<<'\n';
			std::cout<<r.second<<'\n';
			std::cout<<string_to_val(r.second, valsize)<<std::endl;
			std::cout<<*((int*) string_to_val(r.second, valsize))<<std::endl;
			std::cout<<valsize<<std::endl;
			Cache::val_type val = string_to_val((result.find(key))->second, valsize);
			return val;
		} else {
			valsize = 0;
			return nullptr;
		}
	}
	int set(Cache::key_type key, Cache::val_type val, Cache::index_type size){
		std::cout<<"1\n";
		HTTP_request request;
		request.verb = "PUT";
		std::cout<<"2\n";
		request.URI = "/key/" + key + "/" + val_to_string(val, size);
		std::cout << "3\n";
		std::cout<<request.to_string();
		std::cout << "c++ len" << request.to_string().length()<<"\n";
		std::cout<<request.to_string().c_str();
		const char* request_string = (request.to_string()).c_str();
		std::cout<< "c len" << strlen(request_string)<<"\n";
		std::cout << "4\n";
		send(sock, request_string, request.to_string().length(), 0);
		std::cout << "5\n";
		memset(buffer, 0, MAX_MESSAGE_SIZE);
		std::cout << "6\n";
		recv(sock, buffer, MAX_MESSAGE_SIZE, 0);
		std::cout << "7\n";
		HTTP_response response;
		std::cout << "8\n";
		response.parse_raw_response(buffer);
		std::cout << "9\n";
		if (response.code == "200") {
			return 0;
		} else {
			return -1;
		}
	}
	int del(Cache::key_type key){
		HTTP_request request;
		request.verb = "DELETE";
		request.URI = "/key/" + key;
		std::cout<<"requet URI is: " + request.URI + "\n";
		std::cout<<request.to_string();
		auto a = request.to_string();
		const char* request_string = a.c_str();
		std::cout<<"I'm the client, I'm deleting, and my request is: " << request_string<<"\n";
		std::cout<<"length of the message I'm sending: " << request.to_string().length() << "\n";
		std::cout<<"altlen: " << strlen(request_string) << "\n";
		send(sock, request_string, request.to_string().length(), 0);
		memset(buffer, '\0', MAX_MESSAGE_SIZE);
		recv(sock, buffer, MAX_MESSAGE_SIZE, 0);
		HTTP_response response;
		response.parse_raw_response(buffer);
		if (response.code == "200") {
			return 0;
		} else {
			return -1;
		}
	}
	
	Cache::index_type space_used() {
		HTTP_request request;
		request.verb = "GET";
		request.URI = "/memsize";
		const char* request_string = (request.to_string()).c_str();
		send(sock, request_string, request.to_string().length(), 0);
		memset(buffer, '\0', MAX_MESSAGE_SIZE);
		recv(sock, buffer, MAX_MESSAGE_SIZE, 0);
		HTTP_response response;
		response.parse_raw_response(buffer);
		JSON result;
		result.parse_string(response.body);
		unsigned size;
		void* val = string_to_val(result.find("memused")->second, size);
		Cache::index_type to_return = *((Cache::index_type*) val);
		operator delete(val, size);
		return to_return;
	}

};



// construct the cache and consequently the Impl
Cache::Cache(Cache::index_type maxmem, hash_func hasher) 
	: pImpl_(new Impl(maxmem, hasher))
{
}

Cache::~Cache(){
}

// Call set in Impl
int Cache::set(Cache::key_type key, Cache::val_type val, Cache::index_type size){
	return pImpl_->set(key, val, size);
}

// Call get in Impl
Cache::val_type Cache::get(Cache::key_type key, Cache::index_type& size) const{
	return pImpl_->get(key, size);
}

// Call del in Impl
int Cache::del(Cache::key_type key){
	return pImpl_->del(key);
}

// Call space_used in Impl
Cache::index_type Cache::space_used() const{
	return pImpl_->space_used();
}