# include "test.hpp"

using std::string;
using std::cerr;

void printHeader(const string &msg)
{
	cerr << GREEN + msg + RESET;
	cerr << DIVIDER_THICK_BG;
}
