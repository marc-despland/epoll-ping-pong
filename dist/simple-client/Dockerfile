FROM ubuntu
MAINTAINER Marc Despland <marc.despland@orange.com>
RUN apt-get update && apt-get install -y net-tools
RUN mkdir /project
WORKDIR /project
COPY simple-http-client /project/
CMD ./simple-http-client  -p 666 -s server -d