# include <iostream>

# include "WebServer.hpp"

using std::string;
using std::cout;

WebServer::WebServer(const string &config)
{
	_parse(config);
	_init();
	// TO DO: link port number (8080) to config file
	_listener.setUpListener(8080);
	std::cout << "Listener set up !\n";
	_addToPoll(_listener.getListenerFd(), POLLIN, 0);
}

void WebServer::run()
{
	cout << "Web Server started !\n";

	// Event loop with poll()
	while (true)
	{
		// poll() returns the number of file descriptors that have had an event occur on them.
		// return value 0 = timeout, -1 = error
		// -1 passed to poll() will cause the server to wait indefinitely for an event.
		// poll() will block the server but sockets are non-blocking which fulfills the requirement
		// for an event-driven server.
		// .data() returns a pointer to the first element in the array used internally by the vector.
		int ready = poll(_fds.data(), _fds.size(), -1);
		// ready is most commonly zero if polling was interrupted by a signal. safe to retry.
		if (ready < 0)
		{
			cout << "Retrying poll()...\n";
			continue;
		}

		// for loop to service all file descriptors with events
		for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end();)
		{
			// no events detected on the fd
			if (it->revents == 0)
			{
				++it;
				continue;
			}

			// drop problematic/disconnected clients
			// POLLERR = socket error (I/O error or connection reset or unusable socket)
			// POLLHUP = hang up (client disconnected)
			// POLLNVAL = invalid fd (fd closed but still in _fds list)
			if (it->revents & (POLLERR | POLLHUP | POLLNVAL))
			{
				close(it->fd);
				cout << "Closed fd: " << it->fd << "\n";
				// erase() will return the iterator to the next element.
				it = _fds.erase(it);
				continue;
			}

			if (it->revents)



		}






	}
}

void WebServer::_init()
{
	cout << "Initialized Web Server !\n";
}

void WebServer::_parse(const string &config)
{
	_cfg = Config(config);
	cout << "Parsed configuration file !\n";

}

// add a file descriptor and its event to be monitored by poll()
// short is a bitmask for the events to monitor.
void WebServer::_addToPoll(int fd, short events, short revents)
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
	pfd.events = events;
	pfd.revents = revents;
	_fds.push_back(pfd);
}
