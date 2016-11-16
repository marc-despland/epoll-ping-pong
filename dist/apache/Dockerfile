FROM ubuntu
MAINTAINER Marc Despland <marc.despland@orange.com>
RUN apt-get update && apt-get install -y net-tools apache2
RUN mkdir -p /var/run/apache2; mkdir -p /var/lock/apache2
WORKDIR /var/www/html
COPY apache2.conf	/etc/apache2/
COPY envvars	/etc/apache2/
COPY index.html /var/www/html/
COPY mpm_event.conf /etc/apache2/mods-available/
COPY status.conf /etc/apache2/mods-available/
EXPOSE 80
CMD . /etc/apache2/envvars; /usr/sbin/apache2 -DFOREGROUND
