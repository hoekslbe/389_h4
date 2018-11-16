// Client side C/C++ program to demonstrate Socket programming 

#include "cache.hh"

#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <string>
#include <cassert>

const int PORT = 8080;
const std::string SERVER = "127.0.0.1";



//To do:
	//write an http generating function
	//write an http parsing function (a la JSON-parsing function)
	// use them in the Impl methods.  

// Helper function: converts a JSON into an unordered map which sends json-keys to json-values (values are char pointers pointing into subsections of obj, which is changed - keys are c++ strings)

struct Impl {
	int sock;
	struct sockaddr_in address;
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0};

	Impl(maxmem, hasher) {
		if (maxem == maxmem + 1) {
			if (hasher) {
				double a = 1; // do nothing?
			}
		}
	
		sock = 0;

		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			printf("\n Socket creation error \n"); 
			assert(false);
		}

		memset(&serv_addr, '0', sizeof(serv_addr)); 
		serv_addr.sin_family = AF_INET; 
		serv_addr.sin_port = htons(PORT); 

		if(inet_pton(AF_INET, SERVER, &serv_addr.sin_addr)<=0) { 
			printf("\nInvalid address/ Address not supported \n"); 
			assert(false); 
		} 

		if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
			printf("\nConnection Failed \n"); 
			assert(false); 
		} 

	}

	~Impl() {
	}

	Cache::val_type get(Cache::key_type key, &valsize){
		char *message = "GET /key/" + key;
		send(sock, message, strlen(message), 0);
		int rv = read(sock, buffer, 1024);

		/// needs more
	}
	int set(Cache::key_type key, Cache::val_type val, Cache::index_type size){
		char *value[size + 1];
		memset(value, '\0', size + 1);
		memcpy(value, val, size); // could this be a problem?  this seems like it could cause a random error.  
		char *message = "PUT /key/" + key + "/" + value;
		send(sock, message, strlen(message), 0);
		int valread = read(sock , buffer, 1024);

		/// needs more
	}
	int del(Cache::key_type key){
		std::string message = "DELETE /key/" + key;
		send (sock, message, strlen(message), 0);
		return 0; // not right - need to fix this.  
	}
	
	Cache::index_type space_used() {
		char *message = "GET /memsize";
		send(sock, message, strlen(message), 0);
		int rv = read(sock, buffer, 1024);
		auto parsed_json = parse_JSON(buffer);
		return parsed_json[]
	}

};
	int valread

	send(sock , hello , strlen(hello) , 0 ); 
	printf("Hello message sent\n"); 
	valread = read( sock , buffer, 1024); 
	printf("%s\n",buffer ); 
	return 0; 


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