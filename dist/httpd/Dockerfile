FROM ubuntu
MAINTAINER Marc Despland <marc.despland@orange.com>
RUN apt-get update && apt-get install -y net-tools
RUN mkdir /project
WORKDIR /project
COPY httpd /project/
CMD ./httpd  -p 80 -d -k