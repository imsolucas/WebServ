# include <iostream>

# include "signal.hpp"
# include "WebServer.hpp"

using std::string;
using std::cout;

WebServer::WebServer(const string &config)
{
	_parse(config);
	_init();
	// TO DO: link port number (8080) to config file
	_setUpListener(8080);
	std::cout << "Listener set up !\n";
}

WebServer::~WebServer()
{
	for (size_t i = 0; i < _fds.size(); ++i)
		close(_fds[i].fd);
	cout << "Closed all sockets.\n";
}

bool clientConnecting()
{
	return true;
}

void WebServer::run()
{
	cout << "Web Server started !\n";

	int listenerFd = _fds[0].fd;

	// Event loop with poll()
	while (!gStopLoop)
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
		for (size_t i = 0; i < _fds.size(); ++i)
		{
			pollfd & client = _fds[i];
			// std::cout << "FD: " << client.fd << ", REVENTS: " << client.revents << "\n";
			// no events detected on the fd
			if (client.revents == 0)
				continue;

			// drop problematic/disconnected clients
			// POLLERR = socket error (I/O error or connection reset or unusable socket)
			// POLLHUP = hang up (client disconnected)
			// POLLNVAL = invalid fd (fd closed but still in _fds list)
			if ((client.revents & (POLLERR | POLLHUP | POLLNVAL)) && client.fd != listenerFd)
			{
				cout << "Closed fd: " << client.fd << "\n";
				close(client.fd);
				_fds.erase(_fds.begin() + i);
				// decrement to prevent skipping the element that just moved into position i.
				--i;
				continue;
			}

			// POLLIN on listener means client is attempting to connect to the server
			if ((client.revents & POLLIN) && client.fd == listenerFd)
			{
				// create a socket for the client on our server.
				// TODO: pass parameters to accept() to get the client's address and port for caching.
				int clientFd = accept(listenerFd, NULL, NULL);
				// if (clientFd < 0)
				// 	throw SocketCreationException("Failed to create client socket.");
				cout << "Accepted client connection. Client fd: " << clientFd << "\n";
				// set the client's socket to be non-blocking.
				fcntl(clientFd, F_SETFL, O_NONBLOCK);
				_addToPoll(clientFd, POLLIN, 0);
			}

			// POLLIN on client means client is sending data to the server
			else if ((client.revents & POLLIN) && client.fd != listenerFd)
			{
				char buffer[4096];
				// recv() returns the number of bytes read.
				// 0 flag because socket has already been set to be non-blocking
				size_t bytesRead = recv(client.fd, buffer, sizeof(buffer), 0);
				// client POLLIN but 0 bytes read is the client's EOF signal when it closes its connection.
				// a more reliable signal to detect for graceful closure than POLLHUP.
				if (bytesRead == 0)
				{
					close(client.fd);
					cout << "Closed fd: " << client.fd << "\n";
					_fds.erase(_fds.begin() + i);
					// decrement to prevent skipping the element that just moved into position i.
					--i;
					continue;
				}
				else if (bytesRead > 0)
				{
					// process data
					cout << _YELLOW << "Request messages: " << buffer << "\n" << _RESET;
				}
				else
				{
					// handle error
				}

				_sendResponse(client.fd);
			}
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

// TODO: Exception handling
void WebServer::_setUpListener(int port)
{
	// Set up the listener socket

	// AF_INET = internet address (IPv4)
	// SOCK_STREAM = TCP connection
	// 0 = default protocol for the socket type
	int listenerFd = socket(AF_INET, SOCK_STREAM, 0);
	// if (_listener_fd < 0)
	// 	throw SocketCreationException("Failed to create listener socket.");
	cout << "Listener fd: " << listenerFd << "\n";

	// fcntl is used to change the behaviour of the socket
	// F_SETFL = set file status flags
	// set socket to non-blocking mode (call returns immediately if no data in socket)
	// prevent blocking accept/read/write calls
	fcntl(listenerFd, F_SETFL, O_NONBLOCK);

	// setsockopt modifies low-level networking behaviour of the socket.
	// sockets can be modified at the socket (SOL_SOCKET), TCP (IPPROTO_TCP) and IP level (IPPROTO_IP).
	// optval and optlen necessary because setsockopt is a generic API - doesn't always just handle int.
	int opt = 1; // 1 to toggle true/on
	// SO_REUSEADDR = allows a server to bind to an address/port that is in a TIME_WAIT state
	// e.g. when restarting a server quickly and attempting to bind to the same port.
	// OS will typically prevents port binding until the port is fully closed.
	setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// "socket address, internet" stores IP address, port, and family info.
	sockaddr_in listenerAddress;
	listenerAddress.sin_family = AF_INET; // IPv4
	// htons() converts port from host byte order to network byte order
	// Host byte order can be big or little endian. Network byte order is always big endian.
	// htons() ensures that the port number is in the correct byte order for network transmission.
	listenerAddress.sin_port = htons(port);
	// INADDR_ANY means bind to all available interfaces (all local IPs on the machine).
	// (e.g., 127.0.0.1, 192.168.x.x, etc.).
	listenerAddress.sin_addr.s_addr = INADDR_ANY;

	// associates the socket with an address and port on the local machine.
	bind(listenerFd, (sockaddr *)&listenerAddress, sizeof(listenerAddress));

	// listener socket will listen for incoming connection requests.
	// SOMAXCONN is the maximum possible queue size for pending connections.
	listen(listenerFd, SOMAXCONN);

	_addToPoll(listenerFd, POLLIN, 0);
}

void WebServer::_sendResponse(int fd)
{
	// attempt to send a http response.
	// Modern browsers will reuse persistent connections due to HTTP/1.1 keep-alive, which is on by default.
	// So removing "Connection: close" will cause the browser to reuse the same client fd even across
	// different tabs.
	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 13\r\n"
		"Connection: close\r\n"
		"\r\n"
		"Hello, world!";

	// MSG_NOSIGNAL flag prevents SIGPIPE if the client has already closed the connection.
	// Often used in server code to avoid crashes from broken pipes e.g. when client
	// has closed their connection.
	send(fd, response.c_str(), response.size(), MSG_NOSIGNAL);
}


WebServer::WebServerException::WebServerException(std::string err) : _message(err) {}

WebServer::WebServerException::~WebServerException() throw() {}

const char *WebServer::WebServerException::what() const throw()
{
	return _message.c_str();
}

WebServer::SocketCreationException::SocketCreationException(std::string err)
: WebServer::WebServerException("Error: " + err) {}
