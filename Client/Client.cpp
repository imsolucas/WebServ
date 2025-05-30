# include <iostream>
# include <cstring>
# include <cstdlib>
# include <unistd.h>
# include <arpa/inet.h>
# include <sys/socket.h>

# include "Client.hpp"

using std::string;
using std::runtime_error;
using std::cout;

Client::Client(const string &address)
{
	string &ip = _connecting.ip;
	uint16_t &port = _connecting.port;
	sockaddr_in &addr = _connecting.addr;
	socklen_t &len = _connecting.len;

	// parse server address
	ip = address.substr(0, address.find(':'));
	port = atoi(address.substr(address.find(':') + 1).c_str());

	// initialize client socket
	_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket < 0)
		throw runtime_error("Failed to create socket.");

	// initialize server socket address
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

	// connect to server
	if (connect(_socket, reinterpret_cast<sockaddr *>(&addr), len) < 0)
		throw runtime_error("Failed to connect to server.");
}

Client::~Client()
{
	if (close(_socket) < 0)
		throw runtime_error("Failed to close socket.");
}

void Client::request(const string &msg)
{
	ssize_t sent = ::send(_socket, msg.c_str(), msg.size(), 0);
	if (sent < 0)
		throw runtime_error("Failed to send request.");

	// receive response
	char buffer[4096];
	ssize_t bytesReceived = recv(_socket, buffer, 4096, 0);
	if (bytesReceived < 0)
		throw runtime_error("Failed to receive response.");
	cout << "Received " << bytesReceived << " bytes: \n-----\n" << buffer << "\n-----\n";
}

ServerInfo::ServerInfo() : ip(""), port(0), len(sizeof(addr)) { memset(&addr, 0, len); }
