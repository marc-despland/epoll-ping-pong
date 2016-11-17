FROM nginx
MAINTAINER Marc Despland <marc.despland@orange.com>
RUN apt-get update && apt-get install -y net-tools  curl
COPY index.html /usr/share/nginx/html/
COPY nginx.conf /etc/nginx/