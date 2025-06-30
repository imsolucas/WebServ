# include "ClientManager.hpp"
# include "colors.h"
# include "utils.hpp"

# include <fcntl.h>
# include <iostream>
# include <sys/socket.h> // accept, recv, send
# include <unistd.h> // close

using std::cout;
using std::getline;
using std::istringstream;
using std::map;
using std::string;
using std::vector;

ClientManager::ClientManager(std::vector<int> &pollRemoveQueue, std::vector<int> &pollToggleQueue,
								std::vector<int> &pollAddQueue, const std::vector<Server> &servers)
: _pollRemoveQueue(pollRemoveQueue), _pollToggleQueue(pollToggleQueue), _pollAddQueue(pollAddQueue), _servers(servers) {}

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
	_addToClientMap(clientFd, listenerFd, port);
	_pollAddQueue.push_back(clientFd);
	cout << GREEN <<  "Client with fd " << clientFd << " connected on port " << port << "!\n" << RESET;
}

void ClientManager::removeClient(int fd)
{
	close(fd);
	_removeFromClientMap(fd);
	_pollRemoveQueue.push_back(fd);
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
	string response = _handleRequest(_clientMap[fd]);
	// MSG_NOSIGNAL flag prevents SIGPIPE if the client has already closed the connection.
	// Often used in server code to avoid crashes from broken pipes e.g. when client
	// has closed their connection.
	if (send(fd, response.c_str(), response.size(), MSG_NOSIGNAL) <= 0)
	{
		utils::printError("Failed to send response to client with fd " + utils::toString(fd) + ".");
		removeClient(fd);
		return;
	}
	_resetClientMeta(fd);
	_pollToggleQueue.push_back(fd);
}

bool ClientManager::isClient(int fd)
{
	return _clientMap.count(fd);
}

ClientManager::ClientMeta::ClientMeta()
: state(STATE_INIT), listenerFd(0), port(0), server(NULL), requestBuffer()
{
	requestMeta.headersEnd = string::npos;
	requestMeta.chunkedRequest = false;
	requestMeta.contentLength = 0;
	requestMeta.maxBodySizeExceeded = false;
}

ClientManager::PreparsedRequest::PreparsedRequest()
: method() {}

// listenerFd and port are constant across the lifetime of the connection.
// server needs to be reset as the client could send a different Host header.
void ClientManager::_resetClientMeta(int fd)
{
	ClientMeta &client = _clientMap[fd];
	client.state = STATE_INIT;
	client.server = NULL;
	client.requestBuffer.clear();
	client.requestMeta.headersEnd = string::npos;
	client.requestMeta.chunkedRequest = false;
	client.requestMeta.contentLength = 0;
	client.requestMeta.maxBodySizeExceeded = false;
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
				bool success = _preparseHeaders(client);
				if (success)
					client.state = STATE_HEADERS_PREPARSED;
				else
					// if preparsing fails, we assume headers are complete.
					client.state = STATE_REQUEST_READY;
			}
			else
				break;
		}
		// fall through
		case STATE_HEADERS_PREPARSED:
		{
			if (_maxBodySizeExceeded(client))
			{
				client.requestMeta.maxBodySizeExceeded = true;
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
			_pollToggleQueue.push_back(fd);
			break;
		}
		default:
			break;
	}
	if (client.requestMeta.maxBodySizeExceeded)
		_pollToggleQueue.push_back(fd);
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

// This function will return a boolean to indicate whether the headers were successfully preparsed.
bool ClientManager::_preparseHeaders(ClientMeta &client)
{
	PreparsedRequest req;

	const string &headers = client.requestBuffer.substr(0, client.requestMeta.headersEnd);
	istringstream headerStream(headers);
	string line;

	// extract first word from the first line of the request (usually the HTTP method).
	getline(headerStream, line);
	istringstream lineStream(line);
	lineStream >> req.method;

	// extract headers from the request.
	while (getline(headerStream, line))
	{
		line = utils::trim(line, " \t\r");
		if (line.empty())
			continue;
		size_t pos = line.find(':');
		if (pos != string::npos)
		{
			string key = utils::trim(line.substr(0, pos), " \t\r");
			string value = line.substr(pos + 1);
			if (!key.empty() && !value.empty())
				req.headers[utils::toLower(key)] = value;
		}
	}

	// server block must be selected regardless of malformed requests
	// so that handleRequest can construct a response.
	client.server = _selectServerBlock(client, req);

	// validate method and headers for determineBodyEnd to run correctly.
	if (req.method != "GET" && req.method != "POST" && req.method != "DELETE")
		return false;

	if (req.method != "GET") 
	{
		bool hasTE = req.headers.count("transfer-encoding") && 
			req.headers.at("transfer-encoding") == "chunked";
		bool hasCL = req.headers.count("content-length");
		// Transfer-Encoding and Content-Length are mutually exclusive.
		if ((!hasTE && !hasCL) || (hasTE && hasCL))
			return false;
	}

	_determineBodyEnd(client, req);
	return true;
}

// This function will check the http request headers for either the
// "Content-Length" or "Transfer-Encoding" field to determine when
// the http request has been fully received.
// GET requests are complete when the headers are received.
void ClientManager::_determineBodyEnd(ClientMeta &client, const PreparsedRequest &req)
{
	if (req.method == "GET")
		client.requestMeta.contentLength = 0;
	else if (req.headers.count("transfer-encoding"))
		client.requestMeta.chunkedRequest = true;
	else if (req.headers.count("content-length"))
	{
		// create a stringstream to extract the content length as an integer
		std::stringstream ss(req.headers.at("content-length"));
		// set content length to 0 if fail to parse a valid number
		if (!(ss >> client.requestMeta.contentLength))
			client.requestMeta.contentLength = 0;
	}
}

const Server *ClientManager::_selectServerBlock(ClientMeta &client, const PreparsedRequest &req)
{
	string serverName = "";
	if (req.headers.count("host"))
	{
		vector<string> vec = utils::split(req.headers.at("host"), ':');
		// get server name from host header.
		// host value will usually be in the format "example.com:8080".
		// !vec.empty() will prevent out of bounds access if the host header is malformed.
		if (!vec.empty())
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
	size_t maxBodySize = client.server->getClientMaxBodySizeInBytes();
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
