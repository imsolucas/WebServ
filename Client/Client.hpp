# pragma once

# include <iostream>
# include <netinet/in.h>

// Receiving and sending data between a single server
struct ServerInfo
{
	std::string	ip;
	uint16_t	port;
	sockaddr_in	addr;
	socklen_t	len;

	ServerInfo();
};

class Client
{
	public:
		Client(const std::string &address);
		~Client();

		void request(const std::string &msg);

	private:
		int _socket;
		ServerInfo _connecting;
};
