/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etien <etien@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:06:22 by etien             #+#    #+#             */
/*   Updated: 2025/05/21 10:54:33 by etien            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Listener.hpp"

using std::cout;

// TODO: Exception handling
void Listener::setUpListener(int port)
{
	// Set up the listener socket

	// AF_INET = internet address (IPv4)
	// SOCK_STREAM = TCP connection
	// 0 = default protocol for the socket type
	_listenerFd = socket(AF_INET, SOCK_STREAM, 0);
	// if (_listener_fd < 0)
	// 	throw SocketCreationException("Failed to create listener socket.");
	cout << "Listener fd: " << _listenerFd << "\n";

	// fcntl is used to change the behaviour of the socket
	// F_SETFL = set file status flags
	// set socket to non-blocking mode (call returns immediately if no data in socket)
	// prevent blocking accept/read/write calls
	fcntl(_listenerFd, F_SETFL, O_NONBLOCK);

	// setsockopt modifies low-level networking behaviour of the socket.
	// sockets can be modified at the socket (SOL_SOCKET), TCP (IPPROTO_TCP) and IP level (IPPROTO_IP).
	// optval and optlen necessary because setsockopt is a generic API - doesn't always just handle int.
	int opt = 1; // 1 to toggle true/on
	// SO_REUSEADDR = allows a server to bind to an address/port that is in a TIME_WAIT state
	// e.g. when restarting a server quickly and attempting to bind to the same port.
	// OS will typically prevents port binding until the port is fully closed.
	setsockopt(_listenerFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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
