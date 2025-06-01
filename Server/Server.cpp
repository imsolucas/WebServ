# include <iostream>
# include <cstdio>
# include <cstring>
# include <unistd.h>

# include "Server.hpp"

using std::string;
using std::vector;
using std::runtime_error;
using std::cerr;

// initialize listening socket
Server::Server() :
	_state(RUNNING)
{
	cerr << "Initializing server...\n";

	int			&fd = _listener.fd;
	sockaddr_in	&addr = _listener.addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw runtime_error("Failed to create socket.");

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);
	if (bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
		throw runtime_error("Failed to bind socket.");

	cerr << "Finish initializing server.\n";
}

Server::~Server()
{
	// close client connections
	cerr << "Closing server...\n";
	cerr << "Server closed.\n";
}

void Server::run()
{
	// activates the listening socket
	cerr  << "Starting server...\n";
	if (listen(_listener.fd, SOMAXCONN) < 0)
		throw runtime_error("Failed to listen on socket.");
	cerr << "Server started.\n";

	// accepts client connection
	Connection client(_listener.fd);
	_clients.push_back(client);
	cerr << "Connected to client.\n";

	while (_state == RUNNING)
	{
		// receives client request
		string data = client.receiveData();
		if (data.empty())
			throw runtime_error("Failed to receive request.");
		cerr << "Received " << data.size() << " bytes: \n-----\n" << data << "\n-----\n";

		// sends response to cleint
		client.sendData("This is a response.");
		cerr << "Response sent.\n";
	}
}

