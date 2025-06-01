# include <unistd.h>
# include <stdlib.h>

# include "Connection.hpp"

using std::runtime_error;
using std::string;

Connection::Connection(int listener) :
	_state(CONNECTED)
{
	_socket.fd = accept(listener, reinterpret_cast<sockaddr*>(&_socket.addr), &_socket.len);
	if (_socket.fd < 0)
	{
		_state = DISCONNECTED;
		throw runtime_error("Failed to accept connection.");
	}
	memset(_buffer, 0, sizeof(_buffer));
}

Connection::~Connection()
{
	close(_socket.fd);
}

ssize_t Connection::sendData(const string &data)
{
	_state = ACTIVE;
	ssize_t sent = send(_socket.fd, data.c_str(), data.size(), 0);
	if (sent < 0)
		throw runtime_error("Failed to send data.");
	return sent;
}

string Connection::receiveData()
{
	_state = ACTIVE;
	memset(_buffer, 0, sizeof(_buffer));
	ssize_t received = recv(_socket.fd, &_buffer, sizeof(_buffer), 0);
	if (received < 0)
		throw runtime_error("Failed to receive response.");
	if (received == 0)
	{
		_state = DISCONNECTED;
		return "";
	}
	_buffer[received] = '\0';
	return _buffer;
}

int Connection::getFd() const { return _socket.fd; }
ConnectionState Connection::getState() const { return _state; }
void Connection::setState(ConnectionState newState) { _state = newState; }
