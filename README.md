# ft_ping

A *simplified* re-implementation of the `ping` program in C.


## Usage

`./ft_ping [options] <destination>`

where:
- `destination` is the IP address or FQDN to ping
- `options` is one or more options described in the [options](#options) paragraph.


## Options

- `--help` : display the usage command line and the list of options.

- `-p` *tbd*

- `-v` -- *verbose* : enable verbose output.


## Resources

- ping man (GNU inetutils) : https://manpages.debian.org/stretch/inetutils-ping/ping.1.en.html

- Unix network programming, volume 1: The socket networking API (3rd edition) : https://putregai.org/books/unix_netprog_v1.pdf

- IPv4 (Wikipedia page) : https://en.wikipedia.org/wiki/IPv4

- RFC 791 - Internet Protocol : https://www.rfc-editor.org/rfc/rfc791.html

- RFC 792 - Internet Control Message Protocol : https://www.rfc-editor.org/rfc/rfc792.html

- TCP/IP Illustrated, volume 1: The protocols : https://www.r-5.org/files/books/computers/internals/net/Richard_Stevens-TCP-IP_Illustrated-EN.pdf