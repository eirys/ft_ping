FROM debian:latest

COPY ./ft_ping  /usr/bin/ft_ping

RUN apt update -y \
&& apt install -y \
# For debugging
man \
less \
# Ping
inetutils-ping \
&& apt clean -y

CMD ["tail", "-f", "/dev/null"]