# include "WebServer.hpp"
# include "test.hpp"

using std::vector;
using std::string;

static bool test_no_match();
static bool test_exact_match();
static bool test_longest_match();

void test_matchURI(TestSuite &t)
{
	t.addTest(test_no_match);
}

bool test_no_match()
{
	vector<LocationConfig> locations;
	locations.push_back(LocationConfig("/"));
	const string URI = "/unknown/path";
	const LocationConfig &result = WebServer::matchURI(URI, locations);
	return assertEqual("no matching location block", result, locations[0]);
}
