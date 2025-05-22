# include <iostream>
# include <exception>
# include <cstdio>
# include <cstring>
# include <unistd.h>
# include <cerrno>
# include <sys/socket.h>

# include "Server.hpp"

using std::runtime_error;
using std::cout;

void Server::run()
{
	cout  << "Starting server...\n";

	if (listen(_listening.fd, SOMAXCONN) < 0)
		throw runtime_error("Failed to listen on socket.");

	while (_connected.size() != SOMAXCONN)
	{
		Socket client;
		client.fd = accept(_listening.fd, reinterpret_cast<sockaddr *>(&client.addr), &client.len);
		if (client.fd < 0)
		{
			perror("accept");
			continue ;
		}
		_connected.push_back(client);
	}

	cout << "Server started.\n";
}

void Server::init()
{
	cout << "Initializing server...\n";

	if (_connected.size() == SOMAXCONN)
		throw runtime_error("Connection limit reached.");

	int			&fd = _listening.fd;
	sockaddr_in	&addr = _listening.addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	throw runtime_error("Failed to create socket.");
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);
	if (bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
		throw runtime_error("Failed to bind socket.");

	cout << "Finish initializing server.\n";
}
