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

// header file for HTTP conversion utilities written by Robert and documented by Betsy
#include "HTTP_utilities.hh"

const int DEFAULT_MAXMEM = 1024;

// Club penguin is tcp 6113 so we're in good company using this portnum as default if 
 	// no portnum is given in the command args
const int DEFAULT_PORTNUM = 6114;
// A global variable for the state of the server, whether it should be on or not
int running = true;

//This helper function checks to see if a response is empty; it is called at the end of 
//an operation helper function to ensure that function returns a non-empty response.
//A return value of -1 indicates failure, 0 indicates success. 
 bool check_resp(HTTP_response resp) {
 	if((resp.code == " ") && (resp.body == " ")) {
 		std::cout<<"Empty response";
 		return false;
 	}
 	else {
 		return true;
 	}
 }

// This helper function handles the request options following a 'GET' verb
// and returns the proper HTTP response to be packaged and sent to the client
 HTTP_response serv_GET(Cache& cache, HTTP_request& request_det) {
 	// Create empty objects based on HTTP_utilities for an HTTP_response and a JSON string 
 	HTTP_response response;
 	JSON results;
 	// handle GET for memsize
 	if (request_det.URI == "/memsize") {
 		// Find out how much space is used in cache
 		Cache::index_type memsize = cache.space_used();
 		// Make the memsize into response form and set response to it
 		//Cache::index_type * memsize_ptr = &memsize;
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
		std::cout << "key is : " << key << "\n";
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
		std::cout << "key is : " << key << "\n";
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
		std::cout << "key is : " << key << "\n";
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

	std::cout<<"getting out of serv_Del\n";

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
	// PLEASE REMOVE THIS SOMETIME IN THE FUTURE 
	const char* hello = "henlo";
	send(new_socket_fd, hello, strlen(hello), 0);

	// Run an infinite loop to listen for messages from a client
	while (running == true) {
		std::cout<<"\n\n\nNew Read Loop \n\n\n";
		memset(buffer, '\0', MAX_MESSAGE_SIZE);   

		// read the commands from the buffer of client desires using rcv
		int msg = recv(new_socket_fd, buffer, MAX_MESSAGE_SIZE, 0);
		//Cache::index_type val_size = 0;
		if (msg < 0) {
			perror("Failure reading from socket");
			exit(1);
		}
		// loop to examine the buffer
		else {
			// create a new HTTP request and fill it by parsing the characters from the buffer
			// with a function from HTTP_utilities.hh
			if (strlen(buffer) == 0) {
				continue;
			}
			HTTP_request client_request;
			HTTP_response serv_response;
			std::cout<<"buffer is: "<<buffer<<"\n"; // TAKE ME AWAY <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
			std::cout<<"buffer len: " << strlen(buffer) << "\n";
			client_request.parse_raw_request(buffer);

			
			if (client_request.verb == "GET") {
				// call the GET helper if GET requested, two possible types of GET handled with helper
				serv_response = serv_GET(cache, client_request);
			} else if (client_request.verb == "PUT") {
				// call the PUT helper if PUT requested
				serv_response = serv_PUT(cache, client_request);
			} else if (client_request.verb == "DELETE") {
				// call the DEL helper if DELETE requested
				serv_response = serv_DEL(cache, client_request);
			} else if (client_request.verb == "HEAD") {
				// call the HEAD helper if the request is for HEAD to obtain a header 
				serv_response = serv_HEAD(client_request);
			} else if (client_request.verb == "POST") {
				// call the POST helper with server socket, shuts down the server 
				serv_POST(server_socket_fd);
			}
		bool valid_response = check_resp(serv_response);
		if (valid_response == false ) {
			serv_response.body == "There's been an internal error (empty response)";
			serv_response.code == "500";
		}
		// Use the HTTP_utilities to_cstring() method to convert the proper HTTP response into a 
		// string passable to the socket communicating with the client. 
		const char* string_response = (serv_response.to_string()).c_str();
		// Send the response in the proper c string format to the client
		send(new_socket_fd, string_response, serv_response.to_string().length(), 0);	
		// At this point, the loop begins again as the socket continues listening for 
		// requests to fulfill until POST is called to shut down the server.
		}
	}

}