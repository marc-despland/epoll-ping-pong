FROM ubuntu
MAINTAINER Marc Despland <marc.despland@orange.com>
RUN apt-get update && apt-get install -y net-tools
RUN mkdir /project
WORKDIR /project
CMD ./epoll-ping-pong -p 666
COPY epoll-ping-pong /project/