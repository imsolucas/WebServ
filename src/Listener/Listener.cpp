/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etien <etien@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:06:22 by etien             #+#    #+#             */
/*   Updated: 2025/05/20 14:37:52 by etien            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

void Listener::setUpListener(int port)
{
	// Set up the listener socket

	// AF_INET = internet address (IPv4)
	// SOCK_STREAM = TCP connection
	// 0 = default protocol for the socket type
	_listenerFd = socket(AF_INET, SOCK_STREAM, 0);
	// if (_listener_fd < 0)
	// 	throw SocketCreationException("Failed to create listener socket.");

	// fcntl is used to change the behaviour of the socket
	// F_SETFL = set file status flags
	// set socket to non-blocking mode (call returns immediately if no data in socket)
	// prevent blocking accept/read/write calls
	fcntl(_listenerFd, F_SETFL, O_NONBLOCK);

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
	bind(_listenerFd, (sockaddr *)&listenerAddress, sizeof(listenerAddress));

	// listener socket will listen for incoming connection requests.
	// SOMAXCONN is the maximum possible queue size for pending connections.
	listen(_listenerFd, SOMAXCONN);
}

int Listener::getListenerFd() const
{
	return _listenerFd;
}

Listener::ListenerException::ListenerException(std::string err) : _message(err) {}

Listener::ListenerException::~ListenerException() throw() {}

const char *Listener::ListenerException::what() const throw()
{
	return _message.c_str();
}

Listener::SocketCreationException::SocketCreationException(std::string err)
: Listener::ListenerException("Error: " + err) {}
