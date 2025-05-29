# pragma once

# include "Includes.h"

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
