# 389_h4
Homework 5 Benchmarking 

1. Project Goals/System Definition:

A client and a server make up the system. The server stores and operates on a cache object when information is requested by the client. Both processes can occur on a single machine or between designated client and server machines. To measure the performance of the system in the most isolation, we are evaluating the system by testing it on an individual machine acting as both client and server.  

2. System Services:
The server should be able to perform a number of operations on the cache object it manages if the client connected to it makes a request. The following requests are accepted as HTTP messages:

   - GET /key/k: Returns a JSON tuple to client with { key: k, value: v } pair (where k is the resource in the request, and value is the appropriate value from the cache), or an error message if the key isn't in the cache.

   - GET /memsize: Returns a JSON tuple to client with { memused: mem } with the value returned by the Cache's memused method.

   - PUT /key/k/v: create or replace a k,v pair in the cache.

   - DELETE /key/k: Deletes a key, value pair from the cache if the key is present, otherwise does nothing.

   - HEAD /key/k: (Unimplemented at this time) Returns a header to the client regardless of the resource (key) requested. 

   - POST /shutdown: Upon receiving this message, the server stops accepting requests, finishes up any requests in-flight, cleans up the cache (frees resources) and exists cleanly.

System services failure handling:
   Error codes are returned to the user corresponding to the type of failure that occurred if a request is improperly served. 

3. Metrics:

      (a) Latency of requests made by the client 

      (b) Sustained throughput of the system, where system capacity is defined in terms of the maximum offered load (in requests per second) at which the mean response time of the server remains under 1 millisecond

      (c) Accuracy/correctness of sets, gets, deletes performed on the cache

      (d) System bandwidth  

Throughput and bandwidth are closely related. Throughput is bounded by the maximum server response time we want requests to be served in, whereas bandwidth is represented by the maximum amount of data that can be passed through the server in bits per second. 

4. Parameters potentially affecting performance:
   
   System parameters: 

    (a) Overloading of either server or client CPU. Overloading occurs when the server or client CPU runs out of resources, most often processor or RAM resources, and this can occur for a couple of reasons: 

      - Processes running on the machine other than the server-client program limiting available resources 

      - High request load on the network overconsuming processor or RAM resources

    (b) Quality of the network connection: whether server-client program is running on a single computer, wirelessly between two computers, or over ethernet connection

    (c) Electrical power availability: If running on a laptop, is it plugged in? Computer working to preserve battery life can affect performance of system operations.
   
   Workload parameters: 

    (d) Distribution of user requests 
    
    (e) Size of keys and values requested for operation. SET requests for values that exceed the max memory of the cache (1024 bytes) will not be accepted by the server, but beneath the maxmem ceiling, the size of values requested for any operation may affect performance. 
    
    (f) User request frequency
   
5. Factors of Study:
    Client requests
      - SET requests, large and small
      - GET requests, large and small 
      - 

- client "SET" inputs, small and large
- client requests, leveled in a range of frequencies 
//- what other parameters? more challenging parameters to measure/adjust?

6. Evaluation Technique: 
  Our system will be evaluated by simulation because we don't have a real application to test within our client-server system.

7. Workload:
  The workload will consist of a script of queries passed from client to server based on functional analytical models of use patterns from a study of the workload of Memcached. 

8. Experimental Design:
  We will test the performance of our client-server infrastructure by running both processes on a single computer. This will eliminate the overhead latency of network performance that would be present if we tested our system between two or more computers communicating over a network. Because this server is not optimized for multi-threading, if we were to test over a network, performance would only be able to be evaluated between one server-client pairing at a time. By running on a single computer to isolate from network latency we can better understand the raw performance of our system; however, in an actual use scenario, a network would be involved, so performance under this experimental design is somewhat separated from real contexts. Additionally, latency could be more strongly affected by the CPU as it runs both client and server programs and the testing program. 
  We will have 4 test cases, coming from each combination of levels for our parameters:
   1. small keys, small values
   2. small keys, large values
   3. large keys, small values
   4. large keys, large values
   
   For each test case, we will measure the latency of each cache request (GET, SET, DEL, and SPACE_USED), as well as the bandwith of our cache.  
   
   We will determine the latency of each request by measuring the time it takes to make a series of those requests, and then dividing by the number of requests made to get time/request.  We will repeat this process a number of times for each request, and consider the latency of that operation to be the minimum value recorded.  This is because, when measuring latency, we trying to determine the best-case performance of our cache.  
   
   We will model our test workload after the analysis of Memcached's ETC pool workload given in the Memcached workload analysis paper. The paper indicated an approximately 30:1 GET to UPDATE ratio. We take UPDATE in this context to be equivalent to our SET. 
   
   We will determine the overall bandwidth of our cache by measuring the time it takes to make a series of requests according to the distribution presented in the study of Memcached's workload mentioned above, and then dividing the number of requests made by the total time taken to get requests/time.  We will repeat this process a number of times, and consider the cache bandwidth to be the average of all the measurements.  This is because, when measuring bandwidth, we are interested in the average case.  
   
  
  
9. Data Analysis and Interpretation


10. Presentation of Data (meaningful, clear graphs)



Server: The server establishes a socket after parsing the command line requests, and then listens for connections. When data is received through rcv() as a cstring, it is converted to an HTTP request using the HTTP_utilities functionality so that the verb can be parsed, and the request is then handed to a helper function which handles the operations expected from that request verb. 
    HTTP requests handled:
        GET/key/k,
        GET/memsize,
        PUT/key/k/v,
        DELETE/key/k,
        POST/shutdown.
Once the request has been handled by the helper function, an HTTP_response is returned to main representing the results to be passed back to the client. The socket doesn't handle 'HTTP_response's, though, so it must be reformatted using HTTP_utilities again into a JSON string to be compliant with the socket protocol. Once this has happened, the string is passed back to the socket to be sent to the client, and the server continues to listen for requests until the client sends the POST/shutdown request to turn off the server, changing the 'running' variable to stop the request parsing loop. 

Client: 

HTTP_utilities: 
