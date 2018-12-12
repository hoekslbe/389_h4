# 389_h4
Homework 5 Benchmarking 

1. Project Goals/System Definition:

A client and a server make up the system. The server stores and operates on a cache object when information is requested by the client. Both processes can occur on a single machine or between designated client and server machines. To measure the performance of the system in isolation, we are evaluating the system by testing it on an individual machine acting as both client and server. Our system is single-threaded. If it were multithreaded, we would probably choose to test it over a network, to decrease the chance of the single CPU becoming overloaded in passing to and from itself over many threads. 

2. System Services:
The server establishes a socket after parsing the command line requests, and then listens for connections. When data is received through rcv() as a cstring, it is converted to an HTTP request using the HTTP_utilities functionality so that the verb can be parsed, and the request is then handed to a helper function which handles the cache operations expected from that request verb. The following requests are accepted as HTTP messages:

   - GET /key/k: Returns a JSON tuple to client with { key: k, value: v } pair (where k is the resource in the request, and value is the appropriate value from the cache), or an error message if the key isn't in the cache.

   - GET /memsize: Returns a JSON tuple to client with { memused: mem } with the value returned by the Cache's memused method.

   - PUT /key/k/v: create or replace a k,v pair in the cache.

   - DELETE /key/k: Deletes a key, value pair from the cache if the key is present, otherwise does nothing.

   - HEAD /key/k: (Unimplemented at this time) Returns a header to the client regardless of the resource (key) requested. 

   - POST /shutdown: Upon receiving this message, the server stops accepting requests, finishes up any requests in-flight, cleans up the cache (frees resources) and exists cleanly.

Once the request has been handled by the helper function, an HTTP_response is returned to main representing the results to be passed back to the client and/or the success of the operation. The socket doesn't handle 'HTTP_response's, though, so it must be reformatted using HTTP_utilities into a JSON string to be compliant with the TCP socket protocol. The JSON string is then passed back to the socket to be sent to the client, and the server continues to listen for requests until the client sends the POST/shutdown request to turn off the server, which sets the 'running' variable to false to stop the request parsing loop. 

System services failure handling:
   Error codes are returned to the user corresponding to the type of failure that occurred if a request is improperly served. 

3. Metrics:

      (a) Latency of individual request types served

      (b) System bandwidth 
      
Throughput and bandwidth are closely related. Sustained throughput is bounded by the maximum server response time we want requests to be served in, whereas bandwidth is represented by the maximum amount of data that can be passed through the server in bits per second. 
We are not considering sustained throughput a metric here because by our design, when a request is made by the client object, it locks until it has a response from the server, and the server can only maintain a connection to one client thread at a time. Given this, there is no way within our current implementation for the server or client side to send or receive more than one request at a time. Throughput would be measured by the server's response to varying numbers of concurrent client requests. 


4. Parameters potentially affecting performance:
   
   System parameters: 

    (a) Overloading of either server or client CPU. Overloading occurs when the server or client CPU runs out of resources, most often processor or RAM resources, and this can occur for a couple of reasons: 

      - Processes running on the machine other than the server-client program limiting available resources 

      - High request load on the network overconsuming processor or RAM resources 

    (c) Electrical power availability: If running on a laptop, is it plugged in? Computer working to preserve battery life can affect performance of system operations.
   
   Workload parameters: 

    (d) Distribution of user requests 
    
    (e) Size of keys and values requested for operation. SET requests for values that exceed the max memory of the cache (1024 bytes) will not be accepted by the server, but beneath the maxmem ceiling, the size of values requested for any operation may affect performance. 
    
    User request frequency would be an important workload parameter if our server was capable of multi-threading, but in our current implementation, varying request frequency will not affect performance since only one client can communicate one request at a time, locking until the server has responded.
    Quality of the network connection: whether server-client program is running on a single computer, wirelessly between two computers, or over ethernet connection. In our design, network connection will not be a relevant parameter because we will run the system on a single computer.
   
5. Factors of Study:
    Client requests
      - SET , large and small
      - GET , large and small 
      - DEL ,
      - Space_used requests


6. Evaluation Technique: 
  Our system will be evaluated by simulation because we don't have a real application to test within our client-server system.

7. Workload:
  The workload will consist of a script of queries passed from client to server based on functional analytical models of use patterns from a study of the workload of Memcached. 

8. Experimental Design:
  We will test the performance of our client-server infrastructure by running both processes on a single computer. This will eliminate the overhead latency of network performance that would be present if we tested our system between two or more computers communicating over a network. Because this server is not optimized for multi-threading, if we were to test over a network, performance would only be able to be evaluated between one server-client pairing at a time. By running on a single computer to isolate from network latency we can better understand the raw performance of our system; however, in an actual use scenario, a multi-threaded server-client infrastructure operating over a network would be more useful, so performance under this experimental design is somewhat separated from real contexts of application. Additionally, latency could be more strongly affected by the CPU under our design as it handles running the client, server, and testing programs. 
  
  We will have 20 test cases, coming from each combination of levels for our parameters:
   1. small keys, small values
   2. small keys, large values
   3. large keys, small values
   4. large keys, large values
   
   For each test case, we will measure the latency of each cache request (GET, SET, DEL, and SPACE_USED), as well as the bandwith of our cache.  
   
   We will determine the latency of each request by measuring the time it takes to make a series of those requests, and then dividing by the number of requests made to get time/request.  We will repeat this process a number of times for each request, and consider the latency of that operation to be the minimum value recorded.  This is because when measuring latency we are trying to determine the best-case performance of our cache.  
   
   We will model our test workload after the analysis of Memcached's ETC pool workload given in the Memcached workload analysis paper. The paper indicated an approximately 30:1 GET to UPDATE ratio. We take UPDATE in this context to be equivalent to our SET. 
   
   We will determine the overall bandwidth of our cache by measuring the time it takes to make a series of requests according to the distribution presented in the study of Memcached's workload mentioned above, and then dividing the number of requests made by the total time taken to get requests/time.  We will repeat this process a number of times, and consider the cache bandwidth to be the average of all the measurements.  This is because, when measuring bandwidth, we are interested in the average case.  
   
  
  
9. Data Analysis and Interpretation


10. Presentation of Data (meaningful, clear graphs)

