# include "test.hpp"

using std::string;
using std::vector;
using std::cerr;

TestSuite::TestSuite()
{
	cerr << DIVIDER_THICK_BG;
	cerr << BOLD << "TEST SUITE" << RESET;
	cerr << DIVIDER_THICK_BG;
}

void TestSuite::addTest(bool (*t)())
{
	_tests.push_back(t);
}

void TestSuite::run()
{
	bool result;

	for (vector<bool (*)()>::iterator it = _tests.begin();
		it != _tests.end(); ++it)
	{
		result = *it;
		if (result == true) _passes += 1;
		else _fails += 1;
	}
	printSummary(_tests.size(), _passes, _fails);
}

void printHeader(const string &msg)
{
	cerr << GREEN + msg + RESET;
	cerr << DIVIDER_THICK;
}

void printSummary(unsigned int tests, unsigned int passes, unsigned int fails)
{
	cerr << DIVIDER_THICK;
	cerr << BOLD << "Test Summary" << RESET << "\n";
	cerr << DIVIDER_THIN;
	cerr << "Total:   " << tests << " ";
	cerr << "Passed:  " << GREEN << passes << RESET << " ";
	cerr << "Failed:  " << RED << fails << RESET << "\n";
	cerr << DIVIDER_THICK;
}
