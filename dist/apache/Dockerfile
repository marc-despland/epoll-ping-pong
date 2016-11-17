FROM ubuntu
MAINTAINER Marc Despland <marc.despland@orange.com>
RUN apt-get update && apt-get install -y net-tools apache2 curl
RUN mkdir -p /var/run/apache2; mkdir -p /var/lock/apache2
WORKDIR /var/www/html
EXPOSE 80
CMD . /etc/apache2/envvars; /usr/sbin/apache2 -DFOREGROUND
COPY apache2.conf	/etc/apache2/
COPY envvars	/etc/apache2/
COPY index.html /var/www/html/
COPY status.conf /etc/apache2/mods-available/
COPY 000-default.conf /etc/apache2/sites-available/
COPY mpm_event.conf /etc/apache2/mods-available/