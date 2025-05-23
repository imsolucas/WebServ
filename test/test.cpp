# include "test.hpp"

using std::string;
using std::cout;

void printHeader(const string &msg)
{
	cout << GREEN + msg + RESET;
	cout << DIVIDER_THICK;
}
