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

CMD ["tail", "-f", "/dev/null"]