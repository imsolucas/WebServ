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

WebServer::WebServer(const string &config) : _listeners(_poll), _clients(_poll, _pollIndex)
{
	_cfg = Config(config);
	cout << "Parsed configuration file!\n";
	_listeners.setupAllListeners(_cfg.getServers());
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
			throw PollException();

		// for loop to service all file descriptors with events
		for (_pollIndex = 0; _pollIndex < _poll.size(); _pollIndex++)
		{
			pollfd &pfd = _poll[_pollIndex];

			if (_noEvents(pfd))
				continue;
			if (_listeners.isListener(pfd.fd))
				_handleListenerEvents(pfd);
			if (_clients.isClient(pfd.fd))
				_handleClientEvents(pfd);
		}
	}
}

void WebServer::_handleListenerEvents(const pollfd &listener)
{
	if (_listeners.clientIsConnecting(listener))
		_clients.addClient(listener.fd, _listeners.getPort(listener.fd));
}

void WebServer::_handleClientEvents(const pollfd &client)
{
	int fd = client.fd;

	if (_clients.clientIsDisconnected(client))
		_clients.removeClient(fd);
	else if (_clients.clientIsSendingData(client))
		_clients.recvFromClient(fd);
	else if (_clients.clientIsReadyToReceive(client))
		_clients.sendToClient(fd);
}

bool WebServer::_noEvents(const pollfd &pfd)
{
	return pfd.revents == 0;
}

WebServer::PollException::PollException()
: runtime_error("Failed to poll file descriptors.") {}
