# include <iostream>
# include <cstdio>
# include <cstring>
# include <unistd.h>
# include <errno.h>

# include "Server.hpp"

using std::string;
using std::map;
using std::runtime_error;
using std::cerr;

Server::Server() :
	_state(RUNNING)
{
	cerr << "Initializing server...\n";

	// initialize listening socket
	int			&fd = _listener.fd;
	sockaddr_in	&addr = _listener.addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		throw runtime_error("Failed to create socket.");

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(8080);
	int opt = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
		throw runtime_error("Failed to bind socket.");

	if (listen(_listener.fd, SOMAXCONN) < 0)
		throw runtime_error("Failed to listen on socket.");

	// create epoll instance
	_epollInstance = epoll_create1(0);
	if (_epollInstance < 0)
		throw runtime_error("Failed to create epoll instance.");

	// register events for listener
	_registerEvents(_listener.fd, EPOLLIN);

	cerr << "Finish initializing server.\n";
}

Server::~Server()
{
	cerr << "Closing server...\n";

	for (map<int, Connection*>::iterator it = _clients.begin();
		it != _clients.end(); ++it)
		_closeConnection(it->second);
	close(_listener.fd);
	close(_epollInstance);

	cerr << "Server closed.\n";
}

void Server::run()
{
	cerr << "Server started.\n";

	while (_state == RUNNING)
	{
		int triggered = epoll_wait(_epollInstance, events, sizeof(events), -1);
		for (int i = 0; i < triggered; ++i)
		{
			if (events[i].data.fd == _listener.fd)
				_establishConnection();
			else if (_clients.find(events[i].data.fd) != _clients.end())
			{
				if (events[i].events == EPOLLIN)
					_receiveData(_clients[events[i].data.fd]);
				if (events[i].events == EPOLLOUT)
					_sendData(_clients[events[i].data.fd]);
			}
		}
	}
}

void Server::_receiveData(Connection *client)
{
	cerr << "Receiving data from client...\n";

	string data = client->receiveData();
	if (data.empty())
	{
		cerr << "Connection have been closed.\n";
		_closeConnection(client);
		return ;
	}
	_registerEvents(client->getFd(), EPOLLOUT);

	cerr << "Received " << data.size() << " bytes: \n-----\n" << data << "\n-----\n";
}

void Server::_sendData(Connection *client)
{
	cerr << "Sending data to client...\n";

	client->sendData("This is a response.");
	_registerEvents(client->getFd(), EPOLLIN);

	cerr << "Data sent to client.\n";
}

void Server::_establishConnection()
{
	cerr << "Connecting to client...\n";

	if (_clients.size() == SOMAXCONN)
	{
		cerr << "Connection capacity reached.\n";
		return ;
	}

	Connection *client = new Connection(_listener.fd);
	_registerEvents(client->getFd(), EPOLLIN);
	_clients[client->getFd()] =  client;

	cerr << "Connected to client.\n";
}

void Server::_closeConnection(Connection *client)
{
	cerr << "Closing client connection...\n";

	epoll_ctl(_epollInstance, EPOLL_CTL_DEL, client->getFd(), NULL);
	_clients.erase(client->getFd());
	delete client;

	cerr << "Client connection closed.\n";
}

void Server::_registerEvents(int fd, uint32_t events)
{
	cerr << "Registering events...\n";

	epoll_event e;
	e.events = events;
	e.data.fd = fd;
	if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, fd, &e) < 0)
	{
		if (errno == EEXIST)
		{
			if (epoll_ctl(_epollInstance, EPOLL_CTL_MOD, fd, &e) < 0)
				throw runtime_error("Failed to modify existing event registration.");
		}
		else
			throw runtime_error("Failed to register event.");
	}

	cerr << "Events registered.\n";
}
