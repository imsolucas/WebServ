/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Exception.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: imsolucas <imsolucas@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/20 13:44:36 by imsolucas         #+#    #+#             */
/*   Updated: 2025/05/20 13:46:20 by imsolucas        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXCEPTION_HPP
# define EXCEPTION_HPP

#include "Includes.h"

class WebException : public std::exception
{	
	private:
		std::string message;
	public:
		WebException(const std::string &message) : message(message) {}
		virtual ~WebException() throw() {}
		
		virtual const char* what() const throw()
		{
			return this->message.c_str();
		}
};

#endif