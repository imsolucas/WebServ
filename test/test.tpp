# include "colors.h"
# include "test.hpp"

using std::string;
using std::cerr;

// * `==` & `<<` operator overload is required for class/struct objects
// ! this function does not work with pointers
template <typename Output>
void assertEqual(const string &msg, const Output &actual, const Output &expected)
{
	if (actual == expected)
	{
		cerr << "[" << BG_GREEN << "PASS" << RESET << "] ";
		cerr << BOLD + msg << RESET << "\n";
	}
	else
	{
		cerr << "[" + BG_RED + "FAIL" + RESET + "] " + BOLD + msg + RESET;
		cerr << DIVIDER_THIN;
		cerr << YELLOW + "Expected:\n" + RESET;
		cerr << RESET << expected << DIVIDER_THIN;
		cerr << YELLOW + "Actual:\n" + RESET;
		cerr << RESET << actual << DIVIDER_THIN;
	}
}
