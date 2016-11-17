FROM ubuntu
MAINTAINER Marc Despland <marc.despland@orange.com>
RUN apt-get update && apt-get install -y net-tools tcpdump
RUN mkdir /project
WORKDIR /project
COPY keepalive-clients /project/
CMD ./keepalive-clients -c 20000 -m 200 -w 2 -s server -p 80