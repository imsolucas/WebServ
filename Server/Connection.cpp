# include <unistd.h>
# include <stdlib.h>

# include "Connection.hpp"

using std::runtime_error;
using std::string;
using std::cerr;

Connection::Connection(int listener)
{
	_socket.fd = accept(listener, reinterpret_cast<sockaddr*>(&_socket.addr), &_socket.len);
	if (_socket.fd < 0)
		throw runtime_error("Failed to accept connection.");
	int opt = 1;
	setsockopt(_socket.fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(opt));
	memset(_buffer, 0, sizeof(_buffer));
}

Connection::~Connection()
{
	cerr << "wtf\n";
	close(_socket.fd);
	cerr << "wtf1\n";
}

ssize_t Connection::sendData(const string &data)
{
	ssize_t sent = send(_socket.fd, data.c_str(), data.size(), 0);
	if (sent < 0)
		throw runtime_error("Failed to send data.");
	return sent;
}

string Connection::receiveData()
{
	memset(_buffer, 0, sizeof(_buffer));
	ssize_t received = recv(_socket.fd, &_buffer, sizeof(_buffer), 0);
	if (received < 0)
		throw runtime_error("Failed to receive response.");
	if (received == 0)
		return "";
	_buffer[received] = '\0';
	return _buffer;
}

int Connection::getFd() const { return _socket.fd; }
