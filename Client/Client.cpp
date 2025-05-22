# include <iostream>
# include <cstring>
# include <cstdlib>
# include <arpa/inet.h>
# include <sys/socket.h>

# include "Client.hpp"

using std::string;
using std::runtime_error;

Client::Client(const string &address)
{
	string &ip = _connecting.ip;
	uint16_t &port = _connecting.port;
	int &fd = _connecting.fd;
	sockaddr_in &addr = _connecting.addr;

	ip = address.substr(0, address.find(':'));
	port = atoi(address.substr(address.find(':') + 1).c_str());

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw runtime_error("Failed to create socket.");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

	if (connect(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
		throw runtime_error("Failed to connect to server.");
}


Socket::Socket() : fd(0), len(sizeof(addr)) { memset(&addr, 0, len); }
