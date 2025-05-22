# include <iostream>

# include "WebServer.hpp"
# include "signal.hpp"

using std::string;
using std::cout;
using std::cerr;

WebServer::WebServer(const string &config)
{
	_parse(config);
	_init();
	// TODO: link port number to config file
	_setUpListener(8080);
}

WebServer::~WebServer()
{
	closeAllSockets();
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
		for (size_t i = 0; i < _poll.size(); ++i)
		{
			pollfd & socket = _poll[i];
			SocketMeta & socketMeta = _socketMap.find(socket.fd)->second;

			// std::cout << "FD: " << socket.fd << ", REVENTS: " << socket.revents << "\n";

			if (_noEvents(socket))
				continue;

			if (_clientIsDisconnected(socket, socketMeta))
			{
				_removeClient(socket, i);
				// decrement to prevent skipping the element that just moved into position i.
				--i;
				continue;
			}

			if (_clientIsConnecting(socket, socketMeta))
				_addClient(socket);

			else if (_clientIsSendingData(socket, socketMeta))
			{
				bool clientRemoved = _recvFromClient(socket, i);
				if (clientRemoved)
				{
					--i;
					continue;
				}
			}
		}
	}
}

void WebServer::closeAllSockets()
{
	for (size_t i = 0; i < _poll.size(); ++i)
		close(_poll[i].fd);
	cout << "Closed all sockets!\n";
}

void WebServer::_init()
{
	cout << "Initialized web server!\n";
}

void WebServer::_parse(const string &config)
{
	_cfg = Config(config);
	cout << "Parsed configuration file!\n";
}

void WebServer::_removeClient(const pollfd & socket, int i)
{
	int clientFd = socket.fd;

	close(clientFd);
	_removeFromPoll(i);
	_removeFromSocketMap(clientFd);
	cout << RED << "\nClient with fd " << clientFd << " disconnected!\n" << RESET;
}

void WebServer::_addClient(const pollfd & socket)
{
	// create a socket for the client on our server.
	// TODO: pass parameters to accept() to get the client's address and port for caching.
	int clientFd = accept(socket.fd, NULL, NULL);
	if (clientFd < 0)
	{
		printError("Failed to add client.");
		return;
	}
	// set the client's socket to be non-blocking.
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		printError("Failed to set non-blocking mode for client with fd " + std::to_string(socket.fd) + ".\n");
		close(clientFd);
		return;
	}
	_addToPoll(clientFd, POLLIN, 0);
	_addToSocketMap(clientFd, SocketMeta::CLIENT, socket.fd, -1);
	cout << GREEN <<  "\nClient with fd " << clientFd << " connected!\n" << RESET;
}

// true boolean returned means client was removed.
bool WebServer::_recvFromClient(const pollfd & socket, int i)
{
	char buffer[4096];
	// recv() returns the number of bytes read.
	// 0 flag because socket has already been set to be non-blocking.
	size_t bytesReceived = recv(socket.fd, buffer, sizeof(buffer), 0);
	// 0 bytes read is the client's EOF signal when it closes its connection.
	// a more reliable signal to detect for graceful closure than POLLHUP.
	if (bytesReceived == 0)
	{
		cout << "Client with fd " << socket.fd << " has hung up.\n";
		_removeClient(socket, i);
		return true;
	}
	else if (bytesReceived < 0)
	{
		printError("Failed to receive request from client with fd " + std::to_string(socket.fd) + ".\n");
		_removeClient(socket, i);
		return true;
	}
	else
	{
		// process data
		buffer[bytesReceived] = '\0';
		cout << "\nData received from client with fd " << socket.fd <<  ":\n" << YELLOW << buffer << "\n" << _RESET;
		_sendResponse(socket, i);
	}
	return false;
}

void WebServer::_sendResponse(const pollfd & socket, int i)
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
	if (send(socket.fd, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0)
	{
		printError("Failed to send response to client with fd " + std::to_string(socket.fd) + ".\n");
		_removeClient(socket, i);
	}
}

// TODO: Exception handling
void WebServer::_setUpListener(int port)
{
	std::string portString = std::to_string(port);

	// AF_INET = internet address (IPv4)
	// SOCK_STREAM = TCP connection
	// 0 = default protocol for the socket type
	int listenerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenerFd < 0)
		throw SocketCreationException("Failed to create listener socket for port " + portString + ".");

	// fcntl is used to change the behaviour of the socket
	// F_SETFL = set file status flags
	// set socket to non-blocking mode (call returns immediately if no data in socket)
	// prevent blocking accept/read/write calls
	if (fcntl(listenerFd, F_SETFL, O_NONBLOCK) < 0)
		throw SocketConfigException("Failed to set non-blocking mode for listener on port " + portString + ".");

	// setsockopt modifies low-level networking behaviour of the socket.
	// sockets can be modified at the socket (SOL_SOCKET), TCP (IPPROTO_TCP) and IP level (IPPROTO_IP).
	// optval and optlen necessary because setsockopt is a generic API - doesn't always just handle int.
	int opt = 1; // 1 to toggle true/on
	// SO_REUSEADDR = allows a server to bind to an address/port that is in a TIME_WAIT state
	// e.g. when restarting a server quickly and attempting to bind to the same port.
	// OS will typically prevents port binding until the port is fully closed.
	if (setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw SocketOptionException("Failed to set socket option (SO_REUSEADDR) for listener on port " + portString + ".");

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
	if (bind(listenerFd, (sockaddr *)&listenerAddress, sizeof(listenerAddress)) < 0)
		throw BindException("Failed to bind listener socket to port " + portString + ".");


	// listener socket will listen for incoming connection requests.
	// SOMAXCONN is the maximum possible queue size for pending connections.
	if (listen(listenerFd, SOMAXCONN) < 0)
		throw ListenException("Failed to listen to socket on port " + portString + ".");

	_addToPoll(listenerFd, POLLIN, 0);
	_addToSocketMap(listenerFd, SocketMeta::LISTENER, -1, port);

	std::cout << BLUE << "Listener with fd " << listenerFd << " set up on port " << port << "!\n" << RESET;
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
	_poll.push_back(pfd);
}

void WebServer::_removeFromPoll(int i)
{
	_poll.erase(_poll.begin() + i);
}

// listeners: addToSocketMap(listenerFd, SocketMeta::LISTENER, -1, port);
// clients: addToSocketMap(clientFd, SocketMeta::CLIENT, listenerFd, -1);
void WebServer::_addToSocketMap(int fd, SocketMeta::Role type, int listenerFd, int port)
{
	SocketMeta meta;

	meta.type = type;
	meta.listenerFd = listenerFd;
	meta.port = port;
	_socketMap[fd] = meta;
}

void WebServer::_removeFromSocketMap(int fd)
{
	_socketMap.erase(fd);
}

bool WebServer::_isClient(const SocketMeta & socketMeta) const
{
	return (socketMeta.type == SocketMeta::CLIENT);
}

bool WebServer::_isListener(const SocketMeta & socketMeta) const
{
	return (socketMeta.type == SocketMeta::LISTENER);
}

bool WebServer::_noEvents(const pollfd & socket) const
{
	return socket.revents == 0;
}

// problematic/disconnected clients
// POLLERR = socket error (I/O error or connection reset or unusable socket)
// POLLHUP = hang up (client disconnected)
// POLLNVAL = invalid fd (fd closed but still in _poll list)
bool WebServer::_clientIsDisconnected(const pollfd & socket, const SocketMeta & socketMeta) const
{
	return _isClient(socketMeta) && (socket.revents & (POLLERR | POLLHUP | POLLNVAL));
}

// POLLIN on listener means client is attempting to connect to the server
bool WebServer::_clientIsConnecting(const pollfd & socket, const SocketMeta & socketMeta) const
{
	return _isListener(socketMeta) && (socket.revents & POLLIN);
}

// POLLIN on client means client is sending data to the server
bool WebServer::_clientIsSendingData(const pollfd & socket, const SocketMeta & socketMeta) const
{
	return _isClient(socketMeta) && (socket.revents & POLLIN);
}

void printError(std::string message)
{
	cerr << RED << "Error: " << message << RESET;
}

void WebServer::_debugPollAndSocketMap() const
{
	std::cout << "\n===== DEBUG: Current Server State =====\n";

	std::cout << ">> _poll contents:\n";
	for (size_t i = 0; i < _poll.size(); ++i)
	{
		std::cout << "  [" << i << "] FD: " << _poll[i].fd
		          << ", REVENTS: " << _poll[i].revents << "\n";
	}

	std::cout << ">> _socketMap contents:\n";
	for (std::map<int, SocketMeta>::const_iterator it = _socketMap.begin(); it != _socketMap.end(); ++it)
	{
		std::cout << "  FD: " << it->first << ", TYPE: " << (it->second.type == SocketMeta::LISTENER ? "LISTENER" : "CLIENT") << "\n";
	}

	std::cout << "=======================================\n\n";
}


// EXCEPTIONS

const char *WebServer::PollException::what() const throw()
{
	return "Poll failure: poll() < 0";
}

WebServer::SocketCreationException::SocketCreationException(const std::string& msg)
: std::runtime_error(msg) {}

WebServer::SocketConfigException::SocketConfigException(const std::string& msg)
: std::runtime_error(msg) {}

WebServer::SocketOptionException::SocketOptionException(const std::string& msg)
: std::runtime_error(msg) {}

WebServer::BindException::BindException(const std::string& msg)
: std::runtime_error(msg) {}

WebServer::ListenException::ListenException(const std::string& msg)
: std::runtime_error(msg) {}
