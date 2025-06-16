# include <iostream>

# include "colors.h"
# include "test.hpp"

int main()
{
	TestSuite t;
	test_http(t);
	test_matchURI(t);
	test_handleError(t);
	t.run();
}
