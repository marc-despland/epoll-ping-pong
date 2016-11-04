FROM ubuntu
MAINTAINER Marc Despland <marc.despland@orange.com>
RUN apt-get update && apt-get install -y net-tools
RUN mkdir /project
WORKDIR /project
COPY epoll-client /project/
CMD ./epoll-client -p 666 -s server -c 25000