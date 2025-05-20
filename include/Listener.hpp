/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etien <etien@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 12:04:18 by etien             #+#    #+#             */
/*   Updated: 2025/05/20 20:50:24 by etien            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <fcntl.h> // fcntl
# include <iostream>
 #include <netinet/in.h> // htons, sockaddr_in struct
# include <poll.h>
# include <sys/socket.h> // bind, listen, recv, socket
# include <unistd.h> // close
# include <vector>

class Listener
{
	public:
		void setUpListener(int port);
		int getListenerFd() const;

		class ListenerException : public std::exception
		{
			protected:
				std::string _message;

			public:
				ListenerException(std::string err);
				virtual ~ListenerException() throw();
				const char *what() const throw();
		};

		class SocketCreationException : public ListenerException
		{
			public:
				SocketCreationException(std::string err);
		};

	private:
		int _listenerFd;
};
