# pragma once

# include <iostream>

# include "Socket.hpp"

class Connection
{
	public:
		Connection(int listener);
		Connection() {};
		~Connection();

		ssize_t sendData(const std::string &data);
		std::string receiveData();

		int getFd() const;

	private:
		Socket _socket;
		char _buffer[1024];
};
