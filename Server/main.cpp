# include <iostream>

# include "Server.hpp"

using std::exception;
using std::cerr;

int main()
{
	Server s;

	try
	{
		s.init();
		s.run();
	}
	catch(const exception& e)
	{
		cerr << e.what() << '\n';
	}
}
