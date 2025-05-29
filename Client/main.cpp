# include <iostream>

# include "Client.hpp"

using std::exception;
using std::cerr;

int main()
{
	try
	{
		Client c("127.0.0.1:8080");
		c.request("This is a request.");
	}
	catch(const exception& e)
	{
		cerr << e.what() << '\n';
	}
}
