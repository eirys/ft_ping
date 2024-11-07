# ft_ping

A *simplified* re-implementation of the inetutils `ping` program in C.

Send ICMP ECHO_REQUEST packets to network hosts.

## Usage

`./ft_ping [options] <destination>`

where:
- `destination` is the IP address or FQDN to ping
- `options` is one or more options described in the [options](#options) paragraph.


## Options

- `--help` : display the usage command line and the list of options.

- `-v` --*verbose* : enable verbose output.

- `-n` --*numeric* : does not attempt symbolic name lookup for host.

- `-p` --*pattern* : fill packet payload with pattern.

- `-w` --*timeout* : time before the program stops.

- `-i` --*interval* : time to wait before sending a new request.

- `-c` --*count* : number of packets to send (and receive).

- `-s` --*size* : size of payload for the ICMPv4 request.

- `-W` --*linger* : time to wait for a response to an echo request.

- `--ttl` : set time to live (TTL) for each packet.


## Resources

- ping man (GNU inetutils) : https://manpages.debian.org/stretch/inetutils-ping/ping.1.en.html

- Unix network programming, volume 1: The socket networking API (3rd edition) : https://putregai.org/books/unix_netprog_v1.pdf

- IPv4 (Wikipedia page) : https://en.wikipedia.org/wiki/IPv4

- RFC 791 - Internet Protocol : https://www.rfc-editor.org/rfc/rfc791.html

- RFC 792 - Internet Control Message Protocol : https://www.rfc-editor.org/rfc/rfc792.html

- TCP/IP Illustrated, volume 1: The protocols : https://www.r-5.org/files/books/computers/internals/net/Richard_Stevens-TCP-IP_Illustrated-EN.pdf