# pragma once

# include <iostream>
# include <netinet/in.h>

// Receiving and sending data between a single server
struct Server
{
	std::string	ip;
	uint16_t	port;
	sockaddr_in	addr;
	socklen_t	len;

	Server();
};

class Client
{
	public:
		Client(const std::string &address);

		void send(const std::string &msg);

	private:
		int _socket;
		Server _connecting;
};
