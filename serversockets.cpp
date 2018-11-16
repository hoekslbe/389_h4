#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <string.h>
#include "cache.hh"
// header file for HTTP conversion utilities written by Robert and documented by Betsy
#include "HTTP_utilities.hh"

// A global variable for the state of the server, whether it should be on or not
int running = true;

// This helper function handles the request options following a 'GET' verb
// and returns the proper HTTP response to be packaged and sent to the client
 HTTP_response serv_GET(Cache::Cache& cache, HTTP_request& request_det) {
 	// Create empty objects based on HTTP_utilities for an HTTP_response and a JSON string 
 	HTTP_response response;
 	JSON results;
 	// handle GET for memsize
 	if (request_det.uri == "/memsize") {
 		// Find out how much space is used in cache
 		Cache::index_type memsize = cache.space_used();
 		// Make the memsize into response form and set response to it
 		Cache::index_type * memsize_ptr = &memsize;
 		// Call the add method in the JSON struct
 		results.add("memused", (void *) memsize_ptr, sizeof(Cache::index_type));
 		// Cast the JSON back to a string with the JSON tostring method
 		std::string result_string = results.tostring();
 		// Set the body of the HTTP response to the string form of the JSON 
 		response.code = "200";
 		response.body = result_string; 
 		return response;
 	}
 	// Handle GET for a key
 	if (request_det.uri == "/key/k") {
 		// Create a variable to be adjusted by cache get to reflect requested value size
 		Cache::val_size gotten_size;
 		// Call 
 		Cache::val_type key_gotten = cache.get("key", &gotten_size);
 		// set response to right form of key and value gotten 
 		//results.add()
 		return response;
 	}
 	else {
 		// Assign error message for the result of an invalid URI
 		response.code = "502 bad gateway";
 	}
 }
// helper function handling PUT verb requests
 HTTP_response serv_PUT(Cache::Cache& cache, HTTP_request& request_det) {

 	//create or replace a k,v pair in the cache
 	Cache::key_type key;
 	Cache::val_type	val;
 	Cache::index_type set_size;
 	cache.set(key, val, set_size)
 }

// helper function handling DELETE verb requests 
 HTTP_response serv_DEL(Cache::Cache& cache, HTTP_request& request_det) {

 }
 // handle HEAD request, HEAD/key/k, returns just a header in 
 // HTTP_response form regardless of key. Return at least http
 // version, Date, Accept, and Content-Type 
 HTTP_response serv_HEAD(HTTP_request& request_det) {
 	// return a header 
 }

 // takes a reference to the file descriptor for the server socket
 void serv_POST(int& serv_socket) {
 	// turn off further receiving operations to the socket,
 	// but down not disable send operations yet, in case anything
 	// needs to finish
 	// shutdown is from <sys/socket.h>
 	shutdown(serv_socket, SHUT_RD);
 	// make sure all of the requests are filled before shutting down further 
 	// writes with SHUT_WR ? 
 	// exit cleanly?
 }
 
 // Main parses command line arguments indicating maxmem and/or portnum,
 // then uses that information to instantiate a cache and a TCP socket-based server structure.
 // The server 
 int main(int argc, char *argv[]) {
 	int ind = 1;
 	int maxmem = 1024;
 	// Club penguin is tcp 6113 so we're in good company using this portnum as default if 
 	// no portnum is given in the command args
 	int portnum = 6114;

 	// Parse command line arguments for maxmem and portnum inputs
 	while(ind < argc) {
 		// Cast the current command line argument to a c++ string
 		std::string cmd_arg(argv[ind]);
 		// Set maxmem to input if given
 		if ( cmd_arg == "-m") {
 			ind += 1;
 			maxmem = std::stoi(argv[ind]);
 		}
 		// Set portnum to input if given
 		if ( cmd_arg == "-t") {
 			ind += 1;
 			portnum = std::stoi(argv[ind]);
 		}
 		ind += 1;
 	}
 	// Create a cache using the constructor from cache.hh
 	Cache cache(maxmem);
 	// Create a buffer to receive inputs from the client
 	char buffer[1024];
 	// Create variables for sockets and a structure for addresses 
 	int server_socket_fd, new_socket_fd;
	struct sockaddr_in serv_addr, cli_addr;
	int addrlen = sizeof(serv_addr);

 	// Make a call to socket() to create a TCP socket using the 
	// IPv4 protocol in corresponding AF_INET domain
	server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket_fd < 0) {
		perror("Failure opening socket");
		exit(1);
	}

	// Initialize socket structure
	bzero((char*) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portnum);
	// Bind the socket to an address on the server host machine
	if (bind(server_socket_fd, (struct sockaddr *) &serv_addr,
		addrlen) < 0) {
		perror("Failure binding socket");
		exit(1);
	}
	// mark the server socket as listening for connections
	listen(server_socket_fd, 1);
	int clilen = sizeof(cli_addr);
	// accept connection from the queue, forming a new socket
	new_socket_fd = accept(server_socket_fd, (struct sockaddr *)&cli_addr,
		(socklen_t*)&clilen);

	if (new_socket_fd < 0) {
		perror("Failure accepting connection");
		exit(1);
	}
	// This is an infinite loop for receiving input from the client
	while (running == true) {
		bzero(buffer, 1024);

		// read the commands from the buffer of client desires using rcv
		int msg = rcv(new_socket_fd, buffer, 1023, 0);
		//Cache::index_type val_size = 0;
		if (msg < 0) {
			perror("Failure reading from socket");
			exit(1);
		}
		// loop to examine the buffer
		else {
			// create a new HTTP request and fill it by parsing the characters from the buffer
			// with a function from HTTP_utilities.hh
			HTTP_request client_request;
			HTTP_response serv_response;
			client_request.parse_raw_request(buffer);

			// call the GET helper if GET requested, two possible types of GET handled with helper
			if (client_request.verb == "GET") {
				serv_response = serv_get(cache, client_request);
			}
			// call the PUT helper if PUT requested
			if (client_request.verb == "PUT") {
				serv_response = serv_PUT(cache, client_request);
			}
			// call the DEL helper if DELETE requested
			if (client_request.verb == "DELETE") {
				serv_response = serv_DEL(cache, client_request);
			}
			// call the HEAD helper if the request is for HEAD to obtain a header 
			if (client_request.verb == "HEAD") {
				serv_response = serv_HEAD(client_request);
			}
			// call the POST helper with server socket, shuts down the server 
			if (client_request.verb == "POST") {
				serv_POST(server_socket_fd);
			}
		}
	// Use the HTTP_utilities to_cstring() method to convert the proper HTTP response into a 
	// string passable to the socket communicating with the client. 
	char* string_response serv_response.to_cstring();
	// Send the response in the proper c string format to the client
	send(new_socket_fd, string_response, strlen(string_response), 0);	
	// At this point, the loop begins again as the socket continues listening for 
	// requests to fulfill until POST is called to shut down the server.
	}

}