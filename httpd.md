# Single thread server


## Run a load test using **[ab](https://httpd.apache.org/docs/2.4/en/programs/ab.html)**

*ab is a tool for benchmarking your Apache Hypertext Transfer Protocol (HTTP) server*

So to bench our server you just have to start it with the command :
```
$ ./httpd -p 8080
17789 CNXTCP	 NOTICE	 Listening on port 8080
```

And then start the bench with **ab** 
```
$ ab -n 100000. -c 20 http://172.17.0.1:8080/test
This is ApacheBench, Version 2.3 <$Revision: 1430300 $>
Copyright 1996 Adam Twiss, Zeus Technology Ltd, http://www.zeustech.net/
Licensed to The Apache Software Foundation, http://www.apache.org/

Benchmarking 172.17.0.1 (be patient)
(...)
Finished 100000 requests


Server Software:        fast
Server Hostname:        172.17.0.1
Server Port:            8080

Document Path:          /test
Document Length:        13 bytes

Concurrency Level:      20
Time taken for tests:   7.323 seconds
Complete requests:      100000
Failed requests:        0
Write errors:           0
Total transferred:      15400000 bytes
HTML transferred:       1300000 bytes
Requests per second:    13655.48 [#/sec] (mean)
Time per request:       1.465 [ms] (mean)
Time per request:       0.073 [ms] (mean, across all concurrent requests)
Transfer rate:          2053.66 [Kbytes/sec] received
(...)
```

Fot this test, the server **httpd** run on a Docker component, and the **ab** run on the host that run docker. My computer has a bi-processor *Intel(R) Xeon(R) CPU E5-2609 0 @ 2.40GHz* with *64Go* of RAM.

So as *httpd* use the same architecture pattern, I know in this configuration it can handle 1 000 000 simultaneous connections, and we saw here it is capable to manage more than **10 000 request per seconds**