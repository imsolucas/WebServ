# include <iostream>

# include "colors.h"
# include "test.hpp"

int main()
{
	TestSuite t;
	test_serialize(t);
	test_deserialize(t);
	test_handleError(t);
	test_matchURI(t);
	t.run();
}
