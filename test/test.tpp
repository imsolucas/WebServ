# include "colors.h"
# include "test.hpp"

using std::string;
using std::cerr;

// * `==` & `<<` operator overload is required for class/struct objects
template <typename Output>
void assertEqual(const string &msg, const Output &actual, const Output &expected)
{
	if (actual == expected)
	{
		cerr << "[" << BG_GREEN << "PASS" << RESET << "] "
			<< msg << RESET << "\n";
	}
	else
	{
		cerr << "[" + BG_RED + "FAIL" + RESET + "] ";
		cerr << msg + RESET + "\n";
		cerr << YELLOW + "Expected:" + RESET;
		cerr << DIVIDER_THIN;
		cerr << RESET << expected << DIVIDER_THIN;
		cerr << YELLOW + "Actual:" + RESET;
		cerr << DIVIDER_THIN;
		cerr << RESET << actual << DIVIDER_THIN;
	}
}
