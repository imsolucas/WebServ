# include "colors.h"
# include "utils.hpp"
# include "test.hpp"

using std::string;
using std::cerr;

// * `==` & `<<` operator overload is required for class/struct objects
// ! this function does not work with pointers
template <typename Output>
bool assertEqual(const string &msg, const Output &actual, const Output &expected)
{
	if (actual == expected)
	{
		cerr << "[" + BG_GREEN + "PASS" + RESET + "] " + BOLD + msg + "\n" + RESET;
		return true;
	}
	else
	{
		cerr << "[" + BG_RED + "FAIL" + RESET + "] " + BOLD + msg + "\n" + RESET;
		cerr << DIVIDER_THIN + YELLOW + " Expected " + DIVIDER_THIN + "\n" + RESET;
		cerr << expected << "\n";
		cerr << DIVIDER_THIN + YELLOW + " Actual " + DIVIDER_THIN + "\n" + RESET;
		cerr << actual << "\n";
		return false;
	}
}

// * `==` & `<<` operator overload is required for class/struct objects
template <typename Output>
bool assertEqual(const string &msg, const Output *actual, const Output *expected)
{
    // Both null - considered equal
    if (actual == NULL && expected == NULL)
    {
        cerr << "[" + BG_GREEN + "PASS" + RESET + "] " + BOLD + msg + "\n" + RESET;
        return true;
    }

    // One null, other not - not equal
    if (actual == NULL || expected == NULL)
    {
        cerr << "[" + BG_RED + "FAIL" + RESET + "] " + BOLD + msg + "\n" + RESET;
        cerr << DIVIDER_THIN + YELLOW + " Expected " + DIVIDER_THIN + "\n" + RESET;
        cerr << (expected ? "Object: " + utils::toString(*expected) : "NULL") << "\n";
        cerr << DIVIDER_THIN + YELLOW + " Actual " + DIVIDER_THIN + "\n" + RESET;
        cerr << (actual ? "Object: " + utils::toString(*actual) : "NULL") << "\n";
        return false;
    }

    // Both valid - compare values
    if (*actual == *expected)
    {
        cerr << "[" + BG_GREEN + "PASS" + RESET + "] " + BOLD + msg + "\n" + RESET;
        return true;
    }

    cerr << "[" + BG_RED + "FAIL" + RESET + "] " + BOLD + msg + "\n" + RESET;
    cerr << DIVIDER_THIN + YELLOW + " Expected " + DIVIDER_THIN + "\n" + RESET;
    cerr << *expected << "\n";
    cerr << DIVIDER_THIN + YELLOW + " Actual " + DIVIDER_THIN + "\n" + RESET;
    cerr << *actual << "\n";
    return false;
}
