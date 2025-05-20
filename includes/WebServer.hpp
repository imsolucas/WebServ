/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imsolucas <imsolucas@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 14:09:20 by imsolucas         #+#    #+#             */
/*   Updated: 2025/05/20 14:18:19 by imsolucas        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "Includes.h"

class WebServer
{
	public:
		WebServer();
		~WebServer();

		void start();
		void stop();
		void reload();

	private:
		int port;
		std::string server_name;
		std::vector<Server> servers;
};

#endif