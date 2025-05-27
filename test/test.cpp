# include "test.hpp"

using std::string;
using std::vector;
using std::cerr;

TestSuite::TestSuite() :
	_passes(0),
	_fails(0)
{
	printHeader("TEST SUITE");
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
		result = (*it)();
		if (result == true) _passes += 1;
		else _fails += 1;
	}
	printSummary(_tests.size(), _passes, _fails);
}

void printHeader(const string &msg)
{
	cerr << DIVIDER_THICK + GREEN + " " + msg + " " + DIVIDER_THICK + "\n" + RESET;
}

void printSummary(unsigned int tests, unsigned int passes, unsigned int fails)
{
	cerr << DIVIDER_NORMAL + GREEN + " Summary " + DIVIDER_NORMAL + "\n" + RESET;
	cerr << "Total: " << BOLD << tests << RESET + " ";
	cerr << "Passed: " << GREEN << passes << RESET + " ";
	cerr << "Failed: " << RED << fails << RESET;
	cerr << "\n" DIVIDER_THICK DIVIDER_THICK "\n";
}
