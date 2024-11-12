FROM debian:latest

COPY ./ft_ping /home/ft_ping

RUN apt update -y \
&& apt install -yq \
# For debugging
valgrind \
man \
less \
# Ping
inetutils-ping \
# Wireshark
wireshark \
&& apt clean -y

WORKDIR /home

CMD ["tail", "-f", "/dev/null"]