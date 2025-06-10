# include "ClientManager.hpp"
# include "colors.h"
# include "utils.hpp"

# include <fcntl.h>
# include <fstream> // TODO: DELETE
# include <iostream>
# include <sys/socket.h> // accept, recv, send
# include <unistd.h> // close

using std::cout;
using std::string;
using std::vector;

ClientManager::ClientMeta::ClientMeta()
: state(STATE_INIT), listenerFd(0), port(0), server(NULL),
  requestBuffer(), errorCode(0)
{
	requestMeta.headersEnd = string::npos;
	requestMeta.chunkedRequest = false;
	requestMeta.contentLength = 0;
}

ClientManager::ClientManager(std::vector<pollfd> &poll, size_t &pollIndex, const std::vector<Server> &servers)
: _poll(poll), _pollIndex(pollIndex), _servers(servers) {}

void ClientManager::addClient(int listenerFd, int port)
{
	// create a socket for the client on our server.
	int clientFd = accept(listenerFd, NULL, NULL);
	if (clientFd < 0)
	{
		utils::printError("Failed to add client.");
		return;
	}
	// set the client's socket to be non-blocking.
	if (fcntl(clientFd, F_SETFL, O_NONBLOCK) < 0)
	{
		utils::printError("Failed to set non-blocking mode for client with fd " + utils::toString(clientFd) + ".");
		close(clientFd);
		return;
	}
	utils::addToPoll(_poll, clientFd, POLLIN, 0);
	_addToClientMap(clientFd, listenerFd, port);
	cout << GREEN <<  "Client with fd " << clientFd << " connected on port " << port << "!\n" << RESET;
}

void ClientManager::removeClient(int fd)
{
	close(fd);
	utils::removeFromPoll(_poll, _pollIndex);
	_removeFromClientMap(fd);
	// decrement to prevent skipping the element that just moved into position i.
	_pollIndex--;
	cout << RED << "Client with fd " << fd << " disconnected!\n" << RESET;
}

// boolean reflects success of recv call().
void ClientManager::recvFromClient(int fd)
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
		removeClient(fd);
	}
	else if (bytesReceived < 0)
	{
		utils::printError("Failed to receive request from client with fd " + utils::toString(fd) + ".");
		removeClient(fd);
	}
	else
		_handleIncomingData(fd, buffer, bytesReceived);
}

void ClientManager::sendToClient(int fd)
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
		utils::printError("Failed to send response to client with fd " + utils::toString(fd) + ".");
	// remove client regardless whether send succeeded or failed.
	removeClient(fd);
}

bool ClientManager::isClient(int fd)
{
	return _clientMap.count(fd);
}

void ClientManager::_addToClientMap(int fd, int listenerFd, int port)
{
	ClientMeta meta;
	meta.listenerFd = listenerFd;
	meta.port = port;
	_clientMap[fd] = meta;
}

void ClientManager::_removeFromClientMap(int fd)
{
	_clientMap.erase(fd);
}

void ClientManager::_handleIncomingData(int fd, const char *buffer, size_t bytesReceived)
{
	ClientMeta &client = _clientMap[fd];

	client.requestBuffer.append(buffer, bytesReceived);
	cout << "\nData received from client with fd " << fd <<  ":\n" << YELLOW << client.requestBuffer << "\n" << _RESET;

	switch (client.state)
	{
		case STATE_INIT:
			client.state = STATE_RECVING;
		// fall through
		case STATE_RECVING:
		{
			if (_headersAreComplete(client))
			{
				_preparseHeaders(client);
				client.state = STATE_HEADERS_PREPARSED;
			}
			else
				break;
		}
		// fall through
		case STATE_HEADERS_PREPARSED:
		{
			if (_maxBodySizeExceeded(client))
			{
				client.errorCode = 413;
				cout << RED << "Request from client with fd " << fd << " exceeds the maximum body size.\n" << RESET;
				break;
			}
			else if (_bodyIsComplete(client))
				client.state = STATE_REQUEST_READY;
			else
				break;
		}
		// fall through
		case STATE_REQUEST_READY:
		{
			cout << "Received complete request from fd " << fd << ".\n";
			// change poll from POLLIN to POLLOUT to send response to client
			_poll[_pollIndex].events = POLLOUT;
			break;
		}
		default:
			break;
	}
	if (client.errorCode > 0)
	{
		cout << RED << "Error code: " << client.errorCode << ".\n" << RESET;
		_poll[_pollIndex].events = POLLOUT;
	}
}

bool ClientManager::_headersAreComplete(ClientMeta &client)
{
	// in a http request, the end of headers is demarcated by "\r\n\r\n".
	size_t delimiter = client.requestBuffer.find("\r\n\r\n");
	if (delimiter == string::npos)
		return false;
	// +4 to include the "\r\n\r\n" at the end of headers
	client.requestMeta.headersEnd = delimiter + 4;
	return true;
}

void ClientManager::_preparseHeaders(ClientMeta &client)
{
	string headers = client.requestBuffer.substr(0, client.requestMeta.headersEnd);
	HttpRequest req = deserialize(headers);
	_determineBodyEnd(client, req);
	client.server = _selectServerBlock(client, req);
}

// This function will check the http request headers for either the
// "Content-Length" or "Transfer-Encoding" field to determine when
// the http request has been fully received.
// GET requests are complete when the headers are received.
void ClientManager::_determineBodyEnd(ClientMeta &client, const HttpRequest &req)
{
	if (req.method == "GET")
		client.requestMeta.contentLength = 0;
	else if (req.headers.count("Transfer-Encoding"))
	{
		if (req.headers.at("Transfer-Encoding") == "chunked")
			client.requestMeta.chunkedRequest = true;
	}
	else
	{
		if (req.headers.count("Content-Length"))
		{
			// create a stringstream to extract the content length as an integer
			std::stringstream ss(req.headers.at("Content-Length"));
			// set content length to 0 if fail to parse a valid number
			if (!(ss >> client.requestMeta.contentLength))
				client.requestMeta.contentLength = 0;
		}
		else
			client.requestMeta.contentLength = 0;
	}
}

const Server *ClientManager::_selectServerBlock(ClientMeta &client, const HttpRequest &req)
{
	string serverName = "";
	if (req.headers.count("Host"))
	{
		vector<string> vec = utils::split(req.headers.at("Host"), ':');
		// get server name from host header.
		serverName = vec[0];
	}
	// select server candidates with matching ports.
	vector<const Server *> candidates;
	for (vector<Server>::const_iterator serverIt = _servers.begin(); serverIt != _servers.end(); ++serverIt)
	{
		const vector<int> &ports = serverIt->getPorts();
		for (vector<int>::const_iterator portIt = ports.begin(); portIt != ports.end(); ++portIt)
		{
			if (*portIt == client.port)
			{
				candidates.push_back(&(*serverIt));
				break;
			}
		}
	}
	// if only one server matches the port, use it.
	if (candidates.size() == 1)
		return candidates[0];
	// if there are multiple candidates, match based on server name.
	for (vector<const Server *>::const_iterator serverIt = candidates.begin(); serverIt != candidates.end(); ++serverIt)
	{
		const vector<string> &serverNames = (*serverIt)->getServerNames();
		for (std::vector<string>::const_iterator nameIt = serverNames.begin(); nameIt != serverNames.end(); ++nameIt)
		{
			if (*nameIt == serverName)
				return *serverIt;
		}
	}
	// if no server names match, return first candidate.
	return candidates[0];
}

bool ClientManager::_maxBodySizeExceeded(const ClientMeta &client)
{
	size_t maxBodySize = client.server->getClientMaxBodySize();
	size_t bodySize;
	if (client.requestMeta.chunkedRequest)
		bodySize = client.requestBuffer.size() - client.requestMeta.headersEnd;
	else
		bodySize = client.requestMeta.contentLength;
	return bodySize > maxBodySize;
}

bool ClientManager::_bodyIsComplete(const ClientMeta &client)
{
	const size_t headersEnd = client.requestMeta.headersEnd;
	if (client.requestMeta.chunkedRequest)
		// a chunked request ends with "0\r\n\r\n".
		return (client.requestBuffer.find("0\r\n\r\n", headersEnd) != string::npos);
	else
		// a non-chunked request is complete when the buffer can contain
		// the header length + content length.
		return (client.requestBuffer.size() >= headersEnd + client.requestMeta.contentLength);
}
