#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <string.h>
#include "cache.hh"

#include <iostream>

// This is a header file for HTTP conversion utilities we created to simplify server-client interaction implementation
#include "HTTP_utilities.hh"

const int DEFAULT_MAXMEM = 1024;
const int DEFAULT_PORTNUM = 6114;

// A global variable for the state of the server, whether it should be on or not
int running = true;

/* This is called at the end of an operation function to ensure that it
   returns a non-empty response. */ 
 bool check_resp(HTTP_response resp) {
 	if((resp.code == " ") && (resp.body == " ")) {
 		std::cout<<"Empty response";
 		return false;
 	}
 	else {
 		return true;
 	}
 }

// A helper function that handles the request options following a 'GET' verb
// and returns the proper HTTP response to be packaged and sent to the client
 HTTP_response serv_GET(Cache& cache, HTTP_request& request_det) {
 	// Create empty objects based on HTTP_utilities for an HTTP_response and a JSON string 
 	HTTP_response response;
 	JSON results;
 	// handle GET for memsize
 	if (request_det.URI == "/memsize") {
 		// Find out how much space is used in cache
 		Cache::index_type memsize = cache.space_used();
 		// Call the add method in the JSON struct
 		results.add("memused", std::to_string(memsize));
 		// Cast the JSON back to a string with the JSON tostring method
 		std::string result_string = results.to_string();
 		// Set the body of the HTTP response to the string form of the JSON 
 		response.code = "200";
 		response.body = result_string; 
 	} else {
 	// Handle GET for a key
 	if (request_det.URI.substr(0, 5) == "/key/") {
		Cache::key_type key = request_det.URI.substr(5, std::string::npos);
 		// Create a variable to be adjusted by cache get to reflect requested value size
 		Cache::index_type gotten_size;
		//std::cout << "key is : " << key << "\n";
		// Attempt to Get the key 
 		Cache::val_type val_gotten = cache.get(key, gotten_size);
 		// If the get is successful 
		if (val_gotten != nullptr) {
			results.add(key, val_gotten, gotten_size);
			response.body = results.to_string();
			response.code = "200";
		} else {
			response.code = "404";
			response.body = "Item not present in cache";
		}
 	}
 	else {
 		// Assign error message for the result of an invalid URI
 		response.code = "502";
		response.body = "bad gateway";
 	}}
	return response;
 }
// helper function handling PUT verb requests
 HTTP_response serv_PUT(Cache &cache, HTTP_request &request_det) {
	HTTP_response response;

	if (request_det.URI.substr(0, 5) == "/key/") {
	
	 	//create or replace a k,v pair in the cache
		unsigned sep = request_det.URI.find('/', 5);
		Cache::key_type key = request_det.URI.substr(5, sep-5);
		//std::cout << "key is : " << key << "\n";
		std::string val_string = request_det.URI.substr(sep+1, std::string::npos);
 		Cache::index_type set_size;
		void* val = string_to_val(val_string, set_size);
 		if (cache.set(key, val, set_size) < 0) {
			response.code = "500";
			response.body = "Failed to set element";
		} else {
			response.code = "200";
			response.body = "Element successfully set";
		}
		operator delete(val, set_size); // string_to_val allocates memory, so we have to delete here
	} else {
		response.code = "400";
		response.body = "Bad PUT request";
	}
	return response;
 }

// helper function handling DELETE verb requests 
 HTTP_response serv_DEL(Cache &cache, HTTP_request &request_det) {
	HTTP_response response;
	if (request_det.URI.substr(0, 5) == "/key/") {
		Cache::key_type key = request_det.URI.substr(5, std::string::npos);
		//std::cout << "key is : " << key << "\n";
		if (cache.del(key) == 0) {
			response.code = "200";
			response.body = "Element successfully deleted";
		} else {
			response.code = "500";
			response.body = "Failed to delete";
		}
	} else {
		response.code = "400";
		response.body = "Bad DELETE request";
	}

	//std::cout<<"getting out of serv_Del\n";

	return response;
 }
 // handle HEAD request, HEAD/key/k, returns just a header in 
 // HTTP_response form regardless of key. Return at least http
 // version, Date, Accept, and Content-Type 
 HTTP_response serv_HEAD(HTTP_request& request_det) {
	
 	HTTP_response response;
	response.code = "200";
	if (request_det.verb == "Watermelon") { // Just trying to avoid an 'ignoring inputs' compiler error
		response.body = "Jump for Joy!";
	}
	return response; // not implemented to standards
 }

 // takes a reference to the file descriptor for the server socket and shuts down the server. 
 HTTP_response serv_POST(int& serv_socket, HTTP_request& request_det) {
 	// shutdown is from <sys/socket.h>
	HTTP_response response;
 	// shut down receipt of new requests after the shutdown command is given
	if (request_det.URI == "/shutdown") {
 		shutdown(serv_socket, SHUT_RD);
		running = false;
		response.code = "200";
		response.body = "Shutting down";
	} else {
		response.code = "400";
		response.body = "Unsupported POST command";
	}
	return response;
 }
 
 // Main parses command line arguments indicating maxmem and/or portnum,
 // then uses that information to instantiate a cache and a TCP socket-based server structure.
 // The server 
 int main(int argc, char *argv[]) {
 	int ind = 1;
 	int maxmem = DEFAULT_MAXMEM;
 	
 	int portnum = DEFAULT_PORTNUM;

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
 	char buffer[MAX_MESSAGE_SIZE];
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
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portnum);
	// Bind the socket to an address on the server host machine
	if (bind(server_socket_fd, (struct sockaddr *) &serv_addr,
		addrlen) < 0) {
		perror("Failure binding socket");
		exit(1);
	}
	// Mark the server socket as listening for connections
	listen(server_socket_fd, 1);
	int clilen = sizeof(cli_addr);
	// accept connection from the queue, forming a new socket
	new_socket_fd = accept(server_socket_fd, (struct sockaddr *)&cli_addr,
		(socklen_t*)&clilen);

	if (new_socket_fd < 0) {
		perror("Failure accepting connection");
		exit(1);
	}
	// Introductory message upon connection
	const char* hello = "henlo";
	send(new_socket_fd, hello, strlen(hello), 0);

	/* Run an infinite loop to listen for messages from a client. If a shutdown signal is received, 
	running is set to false and the loop stops looking to receive messages. */
	while (running == true) {
		// Zero the buffer in preparation for client messages
		memset(buffer, '\0', MAX_MESSAGE_SIZE);   

		// Read requests in from the buffer that holds client messages
		int msg = recv(new_socket_fd, buffer, MAX_MESSAGE_SIZE, 0);
		if (msg < 0) {
			perror("Failure reading from socket");
			exit(1);
		}
		// Loop to examine the buffer
		else {
			// Check that buffer is not empty. If it is, keep looping for another message,
			// using continue to skip verb parsing since there isn't any verb.
			if (strlen(buffer) == 0) {
				continue;
			}
			// Prep empty HTTP request and response for server-client communications
			HTTP_request client_request;
			HTTP_response serv_response;
			client_request.parse_raw_request(buffer);
			
			/* Check the verb from the client request and call the corresponding helper function,
			which operates on the server cache and returns an HTTP response from the server */
			if (client_request.verb == "GET") {
				// two possible types of GET handled with helper
				serv_response = serv_GET(cache, client_request);
			} else if (client_request.verb == "PUT") {
				serv_response = serv_PUT(cache, client_request);
			} else if (client_request.verb == "DELETE") {
				serv_response = serv_DEL(cache, client_request);
			} else if (client_request.verb == "HEAD") {
				serv_response = serv_HEAD(client_request);
			} else if (client_request.verb == "POST") {
				// serv_POST attempts to close the socket connection and shut down the server. 
				serv_response = serv_POST(server_socket_fd, client_request);
			}
		bool valid_response = check_resp(serv_response);
		if (valid_response == false ) {
			serv_response.body == "There's been an internal error (empty response)";
			serv_response.code == "500";
		}
		/* The socket takes c-strings, so we use the HTTP_utilities to_cstring() method to convert 
		the HTTP response into a string we can pass through the socket. */
		const char* string_response = (serv_response.to_string()).c_str();
		// Send the response in the proper c string format to the client
		send(new_socket_fd, string_response, serv_response.to_string().length(), 0);	
		// At this point, the loop begins again as the socket continues listening for 
		// requests to fulfill until POST is called to shut down the server.
		}
	}

}
