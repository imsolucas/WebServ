# pragma once

# include <iostream>

# include "Socket.hpp"

enum ConnectionState
{
	CONNECTED,		// connection established
	DISCONNECTED,	// client has closed its side of connection
	TERMINATED,		// connection has been shut down and can no longer communicate over it
	ACTIVE,			// connection is actively being used for communication
	IDLE			// connection is open but no activities
};

class Connection
{
	public:
		Connection(int listener);
		~Connection();

		ssize_t sendData(const std::string &data);
		std::string receiveData();

		int getFd() const;

		ConnectionState getState() const;
		void setState(ConnectionState newState);

	private:
		ConnectionState _state;
		Socket _socket;
		char _buffer[1024];
};
