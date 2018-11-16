# 389_h4
Homework 4 networking

We have tried to implement a RESTful web service whereby our client can communicate with our server to request and manipulate information utilizing the cache we created in previous homeworks. We took a tour of libraries for simplifying the manipulation required to convert between c style string socket stream data and HTTP styled responses and requests, but in the end, Pistache was not well documented, so although it was beautiful, we decided not to use it. Boost.beast was well documented but the documentation seemed complex to sort through, so we opted to work with sockets from the ground up. In the end, Robert developed an HTTP_utilities API to simplify JSON, HTTP, and cstring conversion, which we then realized was the point of Pistache, we just couldn't tell exactly how it was working because of the lack of documentation, which made us uncomfortable to rely on it. It is pretty cool to have done our own API, because I think it helped make the server implementation more elegantly organized. 

Server: The server establishes a socket after parsing the command line requests, and then listens for connections. When data is received through rcv() as a cstring, it is converted to an HTTP request using the HTTP_utilities functionality so that the verb can be parsed, and the request is then handed to a helper function which handles the operations expected from that request verb. 
    HTTP requests handled:
        GET/key/k,
        GET/memsize,
        PUT/key/k/v,
        DELETE/key/k,
        HEAD/key/k,
        POST/shutdown.
Once the request has been handled by the helper function, an HTTP_response is returned to main representing the results to be passed back to the client. The socket doesn't handle 'HTTP_response's, though, so it must be reformatted using HTTP_utilities again into a JSON string to be compliant with the socket protocol. Once this has happened, the string is passed back to the socket to be sent to the client, and the server continues to listen for requests until the client sends the POST/shutdown request to turn off the server, changing the 'running' variable to stop the request parsing loop. 

Client: 

HTTP_utilities: 
