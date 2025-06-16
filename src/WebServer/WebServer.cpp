# include "WebServer.hpp"
# include "colors.h"
# include "signal.hpp"
# include "utils.hpp"

# include <iostream>
# include <unistd.h> // close

using std::cerr;
using std::cout;
using std::string;
using std::runtime_error;

WebServer::WebServer(const string &config)
: _servers(_parseConfig(config)), _listenerManager(_poll), _clientManager(_poll, _pollIndex, _servers)
{
	cout << "Parsed configuration file!\n";
	_listenerManager._setupAllListeners(_servers);
}

WebServer::~WebServer()
{
	for (size_t i = 0; i < _poll.size(); ++i)
		close(_poll[i].fd);
	cout << "Closed all sockets!\n";
}

// Event loop with poll()
void WebServer::run()
{
	cout << "Web server started!\n";
	while (!gStopLoop)
	{
		// -1 passed to poll() will cause the server to wait indefinitely for an event.
		// .data() returns a pointer to the first element in the array used internally by the vector.
		int ready = poll(_poll.data(), _poll.size(), -1);
		// poll() returns the number of file descriptors that have had an event occur on them.
		// return value: 0 = timeout (irrelavant with -1 timeout parameter); -1 = error
		// prevent double printing if SIGINT received
		if (ready < 0 && !gStopLoop)
			throw runtime_error("Failed to poll file descriptors.");

		// for loop to service all file descriptors with events
		for (_pollIndex = 0; _pollIndex < _poll.size(); _pollIndex++)
		{
			pollfd &pfd = _poll[_pollIndex];

			if (_noEvents(pfd))
				continue;
			if (_listenerManager.isListener(pfd.fd))
				_handleListenerEvents(pfd);
			if (_clientManager.isClient(pfd.fd))
				_handleClientEvents(pfd);
		}
	}
}

void WebServer::_handleListenerEvents(const pollfd &listener)
{
	if (_clientIsConnecting(listener))
		_clientManager.addClient(listener.fd, _listenerManager.getPort(listener.fd));
}

void WebServer::_handleClientEvents(const pollfd &client)
{
	int fd = client.fd;

	if (_clientIsDisconnected(client))
		_clientManager.removeClient(fd);
	else if (_clientIsSendingData(client))
		_clientManager.recvFromClient(fd);
	else if (_clientIsReadyToReceive(client))
		_clientManager.sendToClient(fd);
}

bool WebServer::_noEvents(const pollfd &pfd)
{
	return pfd.revents == 0;
}

// POLLIN on listener means client is attempting to connect to the server
bool WebServer::_clientIsConnecting(const pollfd &listener)
{
	return listener.revents & POLLIN;
}

// problematic/disconnected clients
// POLLERR = socket error (I/O error or connection reset or unusable socket)
// POLLHUP = hang up (client disconnected)
// POLLNVAL = invalid fd (fd closed but still in _poll list)
bool WebServer::_clientIsDisconnected(const pollfd &client)
{
	return client.revents & (POLLERR | POLLHUP | POLLNVAL);
}

// POLLIN on client means client is sending data to the server
bool WebServer::_clientIsSendingData(const pollfd &client)
{
	return client.revents & POLLIN;
}

// POLLOUT on client means client is ready to receive data
bool WebServer::_clientIsReadyToReceive(const pollfd &client)
{
	return client.revents & POLLOUT;
}

