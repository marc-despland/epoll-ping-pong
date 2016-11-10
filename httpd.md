# Single thread server

## Purpose

This simple httpd server, is not really an HTTP compliant server. It's just an example of a simple single threaded server using [epoll](http://man7.org/linux/man-pages/man7/epoll.7.html) to manage a lot of simultaneous connections. We use the same pattern than our [epoll-server](epoll-server.md) that allow us to manage more than **1 000 000** simultaneous connections.

For *epoll-server* we just keep connections open and play ping-pong with clients. 

Here we read the *HTTP* request until we have a ```\r\n\r\n``` and then we send a small *HTTP* answer and close the connection.

So the idea is just to saw how many request we can manage with this pattern.

## Build

### On Linux host

```
git clone https://github.com/marc-despland/epoll-ping-pong.git
cd epoll-ping-pong
make httpd
```

### On a Docker container

First you have to build an image on your host to build C++ project
```
docker build -t tools/gcc https://github.com/marc-despland/epoll-ping-pong.git#master:/dist/gcc-g++
```

Then you can run create a container  with this image. Assuming you have execute the **git clone** in the folder /projects of your host :
```
docker run -it --rm --name epollproject -v /projects/epoll-ping-pong:/project:Z tools/gcc
root@9ea1c69882cf:/project# make httpd
```

And you will be able to run it in this container. If you want to know which is the IP of this container from your host, simply run :

```
docker inspect --format '{{ .NetworkSettings.IPAddress }}' epollproject
```



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