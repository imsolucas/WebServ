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
	sockaddr_in &addr = _connecting.addr;
	socklen_t &len = _connecting.len;

	ip = address.substr(0, address.find(':'));
	port = atoi(address.substr(address.find(':') + 1).c_str());

	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
		throw runtime_error("Failed to create socket.");

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

	if (connect(_socket, reinterpret_cast<sockaddr *>(&addr), len) < 0)
		throw runtime_error("Failed to connect to server.");
}

void Client::send(const string &msg)
{
	ssize_t sent =	::send(_socket, msg.c_str(), msg.size(), 0);
	if (sent < 0)
		throw runtime_error("Failed to send data to server.");
}

Server::Server() : ip(""), port(0), len(sizeof(addr)) { memset(&addr, 0, len); }
