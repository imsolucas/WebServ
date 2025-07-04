# include "WebServer.hpp"
# include "colors.h"
# include "signal.hpp"
# include "utils.hpp"

# include <iostream>
# include <unistd.h> // close

using std::cout;
using std::map;
using std::runtime_error;
using std::string;
using std::vector;

WebServer::WebServer(const string &config)
: _servers(_parseConfig(config)),
	_clientManager(_pollRemoveQueue, _pollToggleQueue, _pollAddQueue, _servers)
{
	_listenerManager._setupAllListeners(_servers);
	// add listeners to poll
	map<int, int> listenerMap = _listenerManager.getListenerMap();
	for (map<int, int>::iterator it = listenerMap.begin(); it != listenerMap.end(); ++it)
		_addToPoll(it->first);
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
		for (size_t i = 0; i < _poll.size(); ++i)
		{
			pollfd &pfd = _poll[i];

			if (_noEvents(pfd))
				continue;
			if (_listenerManager.isListener(pfd.fd))
				_handleListenerEvents(pfd);
			if (_clientManager.isClient(pfd.fd))
				_handleClientEvents(pfd);
		}
		// poll should only be modified after the loop is completed
		_updatePoll();
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

// process poll queues
// remove -> toggle -> add to minimize search space for remove and modify
void WebServer::_updatePoll()
{
	for (vector<int>::iterator it = _pollRemoveQueue.begin(); it != _pollRemoveQueue.end(); ++it)
		_removeFromPoll(*it);

	for (vector<int>::iterator it = _pollToggleQueue.begin(); it != _pollToggleQueue.end(); ++it)
		_togglePollEvent(*it);

	for (vector<int>::iterator it = _pollAddQueue.begin(); it != _pollAddQueue.end(); ++it)
		_addToPoll(*it);

	_pollRemoveQueue.clear();
	_pollToggleQueue.clear();
	_pollAddQueue.clear();
}

void WebServer::_removeFromPoll(int fd)
{
	for (vector<pollfd>::iterator it = _poll.begin(); it != _poll.end(); ++it)
	{
		if (it->fd == fd)
		{
			_poll.erase(it);
			break;
		}
	}
}

// toggle poll event between POLLIN and POLLOUT
// change poll from POLLIN to POLLOUT to send response to client
void WebServer::_togglePollEvent(int fd)
{
	for (vector<pollfd>::iterator it = _poll.begin(); it != _poll.end(); ++it)
	{
		if (it->fd == fd)
		{
			it->events = (it->events == POLLIN ? POLLOUT : POLLIN);
			break;
		}
	}
}

// add a file descriptor and its event to be monitored by poll(). POLLIN by default.
// short data type for events and revents is a bitmask for the events to monitor.
void WebServer::_addToPoll(int fd)
{
	// A pollfd struct is used to monitor file descriptors for events:
	// 1) FD to monitor
	// 2) EVENTS to monitor (POLLIN = incoming data, POLLOUT = outgoing data)
	//    In case of error, poll() will set error flags in the revents field.
	//    server POLLIN - client wishing to connect
	//    client POLLIN - client wishing to send data
	// 3) REVENTS (returned events which will be set by poll())
	struct pollfd pfd;

	pfd.fd = fd;
	pfd.events = POLLIN;
	pfd.revents = 0;
	_poll.push_back(pfd);
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

