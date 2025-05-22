# pragma once

# include <iostream>
# include <netinet/in.h>

// Receiving and sending data between a single server
struct Socket
{
	std::string	ip;
	uint16_t	port;
	int			fd;
	sockaddr_in	addr;
	socklen_t	len;

	Socket();
};

class Client
{
	public:
		Client(const std::string &address);

	private:
		Socket _connecting;
};
