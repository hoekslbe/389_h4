# 389_h4
Homework 5 Benchmarking 

1. Project Goals/System Definition:
A client and a server make up the system. The server stores and operates on a cache object when information is requested by the client. Both processes can occur on a single machine or between designated client and server machines. To measure the performance of the system in the most isolation, we are evaluating the system by testing it on an individual machine acting as both client and server.  

2. System Services:
The server should be able to perform a number of operations on the cache object it manages if the client connected to it makes a request. The following requests are accepted as HTTP messages:
GET /key/k: Returns a JSON tuple to client with { key: k, value: v } pair (where k is the resource in the request, and value is the appropriate value from the cache), or an error message if the key isn't in the cache.
GET /memsize: Returns a JSON tuple to client with { memused: mem } with the value returned by the Cache's memused method.
PUT /key/k/v: create or replace a k,v pair in the cache.
DELETE /key/k: Deletes a key, value pair from the cache if the key is present, otherwise does nothing.
HEAD /key/k: Returns a header to the client regardless of the resource (key) requested. 
POST /shutdown: Upon receiving this message, the server stops accepting requests, finishes up any requests in-flight, cleans up the cache (frees resources) and exists cleanly.

/// should we put info about possible failures in the system services category as well? the reading sort of implies this 
3. Metrics:
(a) Speed of GET retrieval from server to client
(b) Speed of a single request generally? how much does it differ between set vs get? do we need to test all individually? what's relevant? 
(c) sustained throughput of the system (system capacity defined as the maximum offered load (in requests per second) at which the mean response time of the server remains under 1 millisecond) // sorry this is worded weirdly
(d) accuracy/correctness of sets, gets, deletes

4. Parameters potentially affecting performance:
(a) overloading of either server or client CPU
(b)

5. Factors of Study:
- client "SET" inputs, small and large
- client requests, leveled in a range of frequencies 
//- what other parameters? more challenging parameters to measure/adjust?

6. Evaluation Technique: 
Measurement of a real system instantiated on a single computer. // perhaps some analytical modeling as well?? 

7. Workload:
A script of queries from client to server.

8. Experimental Design:

9. Data Analysis and Interpretation

10. Presentation of Data (meaningful, clear graphs)

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
