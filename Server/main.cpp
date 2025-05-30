# include <iostream>

# include "Server.hpp"

using std::exception;
using std::cerr;

int main()
{
	try
	{
		Server s;
		s.run();
	}
	catch(const exception& e)
	{
		cerr << e.what() << '\n';
	}
}
