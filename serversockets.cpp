#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <string.h>
#include "cache.hh"


 // servercache includes cache.hh and links with cache server 
 int main(int argc, char *argv[]) {
 	int ind = 1;
 	int maxmem = 1024;
 	// club penguin is tcp 6113 so we're in good company if 
 	// no portnum is given in the command args
 	int portnum = 6114;
 	// parse command line arguments for maxmem and portnum inputs
 	while(ind < argc) {
 		// set maxmem to input if input is given
 		if (argv[ind] == "-m") {
 			ind += 1;
 			maxmem = std::stoi(argv[ind]);
 		}
 		if (argv[ind] == "-t") {
 			ind += 1;
 			portnum = std::stoi(argv[ind]);
 		}
 		ind += 1;
 	}
 	// create a cache using the constructor from cache.hh
 	Cache cache(maxmem);
 	char buffer[1024];
 	// create variable for socket file desc
 	int server_socket_fd, new_socket_fd, cli_length;
	struct sockaddr_in serv_addr, cli_addr;
	int opt = 1;
	int addrlen = sizeof(serv_addr);

 	// make a call to create a TCP socket using the 
	// IPv4 protocol in corresponding AF_INET domain
	if ((server_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Failure opening socket");
		// what to do upon exit?
		exit(1);
	}
	// set any socket options with setsockopt?
	//if (setsockopt(server_socket_fd))

	// initialize socket structure
	bzero((char*) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portnum);
	// bind the circuit to an address on the host machine
	if (bind(server_socket_fd, (struct sockaddr_in *) &serv_addr,
		addrlen) < 0) {
		perror("Failure binding socket");
		exit(1);
	}
	// mark the server socket as listening for connections
	listen(server_socket_fd, 1);
	int clilen = sizeof(cli_addr);
	// accept connection from the queue, forming a new socket
	new_socket_fd = accept(server_socket_fd, (struct sockaddr_in*)&cli_addr,
		&clilen);

	if (new_socket_fd < 0) {
		perror("Failure accepting connection");
		exit(1);
	}

	// loop for requests from the client
	while {
		bzero(buffer, 1024);
		int msg = read( new_socket_fd, buffer,1023);
		index_type val_size = 0
		if (msg < 0) {
			perror("Failure reading from socket");
			exit(1);
		}
	// 	while (msg > 0) {
	// 		// use strtok to split the buffer into tokens by " "
	// 		token = strtok(buffer[0]," ")
	// 		std::string request;
	// 		std::string path;

	// 		if (token == "GET") {
	// 			// GET/key/k
	// 			if (next is /key/k) {
	// 				// get with key, val_size
	// 				val_type result = cache.get(key, val_size);
	// 			}
	// 			//path = /key/k
	// 		}
	// 		if (token == "PUT") {

	// 		}
	// 		if (token == "DELETE") {

	// 		}
	// 		if (token == "HEAD") {

	// 		}
	// 		if (token == "POST")
	// 	}	

	}
	


}