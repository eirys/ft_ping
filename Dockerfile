FROM debian:latest

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

WORKDIR /home/ping_output

CMD ["tail", "-f", "/dev/null"]