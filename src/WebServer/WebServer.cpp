# include "WebServer.hpp"
# include "CGIHandler.hpp" // TODO: DELETE
# include "colors.h"
# include "signal.hpp"
# include "utils.hpp"

# include <fcntl.h>
# include <iostream>
# include <netinet/in.h> // htons, sockaddr_in struct
# include <sys/socket.h> // accept, bind, listen, recv, setsockopt, socket
# include <unistd.h> // close

using std::cerr;
using std::cout;
using std::string;
using std::runtime_error;

WebServer::WebServer(const string &config)
{
	_cfg = Config(config);
	cout << "Parsed configuration file!\n";
	_setupAllListeners();
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
			if (_isListener(pfd.fd))
				_handleListenerEvents(pfd);
			if (_isClient(pfd.fd))
				_handleClientEvents(pfd);
		}
	}
}

void WebServer::_setupAllListeners()
{
	std::vector<Server>servers = _cfg.getServers();
	std::vector<Server>::const_iterator serverIt = servers.begin();
	for (; serverIt != servers.end(); ++serverIt)
	{
		std::vector<int>ports = serverIt->getPorts();
		std::vector<int>::const_iterator portIt = ports.begin();
		for (; portIt != ports.end(); ++portIt)
			_setUpListener(*portIt);
	}
}

void WebServer::_setUpListener(int port)
{
	string portString = utils::toString(port);

	// AF_INET = internet address (IPv4)
	// SOCK_STREAM = TCP connection
	// 0 = default protocol for the socket type
	int listenerFd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenerFd < 0)
		throw SocketCreationException(portString);

	// fcntl is used to change the behaviour of the socket.
	// F_SETFL = set file status flags
	// set socket to non-blocking mode (call returns immediately if no data in socket).
	// prevent blocking accept/read/write calls.
	if (fcntl(listenerFd, F_SETFL, O_NONBLOCK) < 0)
	{
		close(listenerFd);
		throw SocketConfigException(portString);
	}

	// setsockopt modifies low-level networking behaviour of the socket.
	// sockets can be modified at the socket (SOL_SOCKET), TCP (IPPROTO_TCP) and IP level (IPPROTO_IP).
	// optval and optlen necessary because setsockopt is a generic API - doesn't always just handle int.
	int opt = 1; // 1 to toggle true/on
	// SO_REUSEADDR = allows a server to bind to an address/port that is in a TIME_WAIT state
	// e.g. when restarting a server quickly and attempting to bind to the same port.
	// OS will typically prevents port binding until the port is fully closed.
	if (setsockopt(listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		close(listenerFd);
		throw SocketOptionException(portString);
	}

	// "socket address, internet" stores IP address, port, and family info.
	sockaddr_in listenerAddress;
	listenerAddress.sin_family = AF_INET; // IPv4
	// htons() converts port from host byte order to network byte order.
	// Host byte order can be big or little endian. Network byte order is always big endian.
	// htons() ensures that the port number is in the correct byte order for network transmission.
	listenerAddress.sin_port = htons(port);
	// INADDR_ANY means bind to all available interfaces (all local IPs on the machine).
	// (e.g., 127.0.0.1, 192.168.x.x, etc.).
	listenerAddress.sin_addr.s_addr = INADDR_ANY;

	// associates the socket with an address and port on the local machine.
	if (bind(listenerFd, (sockaddr *)&listenerAddress, sizeof(listenerAddress)) < 0)
	{
		close(listenerFd);
		throw BindException(portString);
	}

	// listener socket will listen for incoming connection requests.
	// SOMAXCONN is the maximum possible queue size for pending connections.
	if (listen(listenerFd, SOMAXCONN) < 0)
	{
		close(listenerFd);
		throw ListenException(portString);
	}
	_addToPoll(listenerFd, POLLIN, 0);
	_listenerMap[listenerFd] = port;

	cout << BLUE << "Listener with fd " << listenerFd << " set up on port " << port << "!\n" << RESET;
}

void WebServer::_handleListenerEvents(const pollfd &listener)
{
	if (_clientIsConnecting(listener))
		_addClient(listener.fd);
}

void WebServer::_handleClientEvents(const pollfd &client)
{
	int fd = client.fd;

	if (_clientIsDisconnected(client))
		_removeClient(fd);
	else if (_clientIsSendingData(client))
		_recvFromClient(fd);
	else if (_clientIsReadyToReceive(client))
		_sendToClient(fd);
}

void WebServer::_addClient(int listenerFd)
{
	// create a socket for the client on our server.
	int clientFd = accept(listenerFd, NULL, NULL);
	if (clientFd < 0)
	{
		printError("Failed to add client.");
		return;
	}
	// set the client's socket to be non-blocking.
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		printError("Failed to set non-blocking mode for client with fd " + utils::toString(clientFd) + ".\n");
		close(clientFd);
		return;
	}
	int port =  _listenerMap[listenerFd];
	_addToPoll(clientFd, POLLIN, 0);
	_addToClientMap(clientFd, port, listenerFd);
	cout << GREEN <<  "Client with fd " << clientFd << " connected on port " << port << "!\n" << RESET;
}

void WebServer::_removeClient(int fd)
{
	close(fd);
	_removeFromPoll(_pollIndex);
	_removeFromClientMap(fd);
	// decrement to prevent skipping the element that just moved into position i.
	_pollIndex--;
	cout << RED << "Client with fd " << fd << " disconnected!\n" << RESET;
}

// boolean reflects success of recv call().
void WebServer::_recvFromClient(int fd)
{
	char buffer[4096];
	// recv() returns the number of bytes read.
	// 0 flag because socket has already been set to be non-blocking.
	// signed size_t can be negative.
	ssize_t bytesReceived = recv(fd, buffer, sizeof(buffer), 0);
	// 0 bytes read is the client's EOF signal when it closes its connection.
	// a more reliable signal to detect for graceful closure than POLLHUP.
	if (bytesReceived == 0)
	{
		cout << "Client with fd " << fd << " has hung up.\n";
		_removeClient(fd);
	}
	else if (bytesReceived < 0)
	{
		printError("Failed to receive request from client with fd " + utils::toString(fd) + ".\n");
		_removeClient(fd);
	}
	else
	{
		ClientMeta &client = _clientMap[fd];
		client.requestBuffer.append(buffer, bytesReceived);
		// change poll from POLLIN to POLLOUT to send response to client
		cout << "\nData received from client with fd " << fd <<  ":\n" << YELLOW << client.requestBuffer << "\n" << _RESET;
		if (_requestIsComplete(client))
		{
			cout << "Received complete request from fd " << fd << ".\n";
			CGIHandler::testCGIHandler(client.requestBuffer); // TODO: DELETE
			_poll[_pollIndex].events = POLLOUT;
		}
	}
}

// This function will check the http request headers for either the
// "Content-Length" or "Transfer-Encoding" field to determine when
// the http request has been fully received.
// GET requests are complete when the headers are received.
bool WebServer::_requestIsComplete(ClientMeta &client)
{
	// in a http request, the end of headers is demarcated by "\r\n\r\n".
	size_t headersEnd = client.requestBuffer.find("\r\n\r\n");
	// cout << "[DEBUG] Buffer size: " << client.requestBuffer.size() << ", headersEnd: " << headersEnd << "\n";
	// cout << "[DEBUG] Buffer: " << client.requestBuffer << "\n";

	// headersParsed check to ensure code block is only entered once.
	if (!client.headersParsed && headersEnd != string::npos)
	{
		client.headersParsed = true;
		string headers = client.requestBuffer.substr(0, headersEnd);

		// GET requests do not have a body, so they are complete when the headers are received.
		if (headers.find("GET ") == 0)
			return true;

		if (headers.find("Transfer-Encoding: chunked") != string::npos)
			client.chunkedRequest = true;
		else
		{
			string nonChunkedMarker = "Content-Length:";
			size_t pos = headers.find(nonChunkedMarker);
			if (pos != string::npos)
			{
				pos += nonChunkedMarker.size();
				// create a stringstream to extract the content length as an integer
				std::stringstream ss(headers.substr(pos));
				// set content length to 0 if fail to parse a valid number
				if (!(ss >> client.contentLength))
					client.contentLength = 0;
			}
			else
				client.contentLength = 0;
		}
	}

	if (client.headersParsed)
	{
		if (client.chunkedRequest)
		{
			// a chunked request ends with "0\r\n\r\n".
			if (client.requestBuffer.find("0\r\n\r\n", headersEnd) != string::npos)
				return true;
		}
		else
		{
			// a non-chunked request is complete when the buffer can contain
			// the header length + "\r\n\r\n" + content length.
			if (client.requestBuffer.size() >= headersEnd + 4 + client.contentLength)
				return true;
		}
	}
	return false;
}

void WebServer::_sendToClient(int fd)
{
	// TODO: DELETE - ONLY FOR TESTING PURPOSES
	// -----------------------------------------------------------------------------------
	std::ifstream page("public/upload.html");
	string line, body;

	while (std::getline(page, line))
	body += line + "\n";

	// attempt to send a http response.
	// Modern browsers will reuse persistent connections due to HTTP/1.1 keep-alive, which is on by default.
	// So removing "Connection: close" will cause the browser to reuse the same client fd even across
	// different tabs.
	string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + utils::toString(body.size()) + "\r\n"
		"Connection: close\r\n"
		"\r\n" +
		body;
	// -----------------------------------------------------------------------------------

	// MSG_NOSIGNAL flag prevents SIGPIPE if the client has already closed the connection.
	// Often used in server code to avoid crashes from broken pipes e.g. when client
	// has closed their connection.
	if (send(fd, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0)
		printError("Failed to send response to client with fd " + utils::toString(fd) + ".\n");
	// remove client regardless whether send succeeded or failed.
	_removeClient(fd);
}

// HELPER FUNCTIONS

// add a file descriptor and its event to be monitored by poll().
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

void WebServer::_addToClientMap(int fd, int port, int listenerFd)
{
	ClientMeta meta;

	meta.port = port;
	meta.listenerFd = listenerFd;
	meta.requestBuffer = "";
	meta.headersParsed = false;
	meta.chunkedRequest = false;
	meta.contentLength = -1; // default to -1 to avoid confusion
	_clientMap[fd] = meta;
}

void WebServer::_removeFromClientMap(int fd)
{
	_clientMap.erase(fd);
}

bool WebServer::_isClient(int fd)
{
	return _clientMap.count(fd);
}

bool WebServer::_isListener(int fd)
{
	return _listenerMap.count(fd);
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

void WebServer::printError(string message)
{
	cerr << RED << "Error: " << message << RESET;
}

// EXCEPTIONS

const char *WebServer::PollException::what() const throw()
{
	return "Failed to poll file descriptors.";
}

WebServer::SocketCreationException::SocketCreationException(const string &portString)
: runtime_error("Failed to create listener socket for port " + portString + ".") {}

WebServer::SocketConfigException::SocketConfigException(const string &portString)
: runtime_error("Failed to set non-blocking mode for listener on port " + portString + ".") {}

WebServer::SocketOptionException::SocketOptionException(const string &portString)
: runtime_error("Failed to set socket option (SO_REUSEADDR) for listener on port " + portString + ".") {}

WebServer::BindException::BindException(const string &portString)
: runtime_error("Failed to bind listener socket to port " + portString + ".") {}

WebServer::ListenException::ListenException(const string &portString)
: runtime_error("Failed to listen to socket on port " + portString + ".") {}
