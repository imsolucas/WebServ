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

	cerr << "Finish initializing server.\n";
}

Server::~Server()
{
	// close client connections
	cerr << "Closing server...\n";
	for (vector<Socket>::iterator it = _connected.begin(); it != _connected.end(); ++it)
		if (close((*it).fd) < 0)
			perror("Failed to close socket");
	cerr << "Server closed.\n";
}

void Server::run()
{
	// activates the listening socket
	cerr  << "Starting server...\n";
	if (listen(_listening.fd, SOMAXCONN) < 0)
		throw runtime_error("Failed to listen on socket.");
	cerr << "Server started.\n";

	// accepts client connection
	Socket client;
	client.fd = accept(_listening.fd, reinterpret_cast<sockaddr *>(&client.addr), &client.len);
	if (client.fd < 0)
		throw runtime_error("Failed to connect to client.");
	_connected.push_back(client);
	cerr << "Connected to client.\n";

	while (_state == RUNNING)
	{
		// receives client request
		char buffer[4096];
		size_t bytesReceived = recv(client.fd, buffer, 4096, 0);
		if (bytesReceived < 0)
			throw runtime_error("Failed to receive request.");
		cerr << "Received " << bytesReceived << " bytes: \n-----\n" << buffer << "\n-----\n";

		// sends response to cleint
		cerr << "Sending response...\n";
		try
		{
			_respond(client, "This is a response.");
		}
		catch (const runtime_error &e)
		{
			cerr << "Error: " << e.what() << '\n';
		}
		cerr << "Response sent.\n";
	}
}

void Server::_respond(const Socket &client, const string &msg)
{
	ssize_t bytesSent = send(client.fd, msg.c_str(), msg.size(), 0);
	if (bytesSent < 0)
		throw runtime_error("Failed to send response.");
	if (bytesSent == 0)
		throw runtime_error("Client closed the connection.");
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
