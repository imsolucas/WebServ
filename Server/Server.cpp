# include <iostream>
# include <cstdio>
# include <cstring>

# include "Server.hpp"

using std::string;
using std::runtime_error;
using std::cout;

void Server::run()
{
	cout  << "Starting server...\n";

	if (listen(_listening.fd, SOMAXCONN) < 0)
		throw runtime_error("Failed to listen on socket.");

	cout << "Server started.\n";

	Socket client;
	client.fd = accept(_listening.fd, reinterpret_cast<sockaddr *>(&client.addr), &client.len);
	if (client.fd < 0)
		throw runtime_error("Failed to connect to client.");
	cout << "Connected to client.\n";

	_connected.push_back(client);

	char buffer[4096];
	size_t len;
	len = recv(client.fd, buffer, 4096, 0);
	cout << "Received " << len << " bytes: \n-----\n" << buffer << "\n-----\n";

	cout << "Sending response...\n";
	_response("This is a response.");
	cout << "Response sent.\n";

	cout << "Closing server...\n";
	cout << "Server closed.\n";
}

// initialize listening socket
void Server::init()
{
	cout << "Initializing server...\n";

	int			&fd = _listening.fd;
	sockaddr_in	&addr = _listening.addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw runtime_error("Failed to create socket.");

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);
	if (bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
		throw runtime_error("Failed to bind socket.");

	cout << "Finish initializing server.\n";
}

void Server::_response(const string &msg)
{
	ssize_t bytesSent = send(_connected[0].fd, msg.c_str(), msg.size(), 0);
	if (bytesSent < 0)
		throw (runtime_error("Failed to send response."));
}

void Server::_listen()
{
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
}

Socket::Socket() : fd(0), len(sizeof(addr)) { memset(&addr, 0, len); }
