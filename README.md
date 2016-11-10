# epoll-ping-pong
This project is designed to make some tests with [epoll]() and check how many simultaneous TCP connection it is possible to handle on a single server.


# HTTPD
The [HTTPD](./httpd.md) server use the same pattern than the [epoll-server](./epoll-server.md) but this with *HTTP like* connections closing the TCP socket after each response (like using HTTP/1.0).