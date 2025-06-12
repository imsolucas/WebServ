# include "WebServer.hpp"
# include "test.hpp"

using std::vector;
using std::string;

static bool test_no_match();
static bool test_no_match1();
static bool test_exact_match();
static bool test_longest_match();
static bool test_root_match();
static bool test_multiple_matches();

void test_matchURI(TestSuite &t)
{
	t.addTest(test_no_match);
	t.addTest(test_no_match1);
	t.addTest(test_exact_match);
	t.addTest(test_longest_match);
	t.addTest(test_root_match);
	t.addTest(test_multiple_matches);
}

bool test_no_match()
{
	vector<Location> locations;
	locations.push_back(Location("/"));
	const string URI = "/unknown/path";
	const Location &result = WebServer::matchURI(URI, locations);
	return assertEqual("no matching location block", result, locations[0]);
}

bool test_no_match1()
{
	vector<Location> locations;
	locations.push_back(Location("/"));
	locations.push_back(Location("/abc"));
	const string URI = "/abcd";
	const Location &result = WebServer::matchURI(URI, locations);
	return assertEqual("no matching location block", result, locations[0]);
}

bool test_exact_match()
{
	vector<Location> locations;
	locations.push_back(Location("/exact"));
	const string URI = "/exact";
	const Location &result = WebServer::matchURI(URI, locations);
	return assertEqual("location block exact match", result, locations[0]);
}

bool test_longest_match()
{
	vector<Location> locations;
	locations.push_back(Location("/"));
	locations.push_back(Location("/long"));
	locations.push_back(Location("/longer"));
	const string URI = "/longer/path";
	const Location &result = WebServer::matchURI(URI, locations);
	return assertEqual("location block longest prefix match", result, locations[2]);
}

bool test_root_match()
{
	vector<Location> locations;
	locations.push_back(Location("/"));
	const string URI = "/";
	const Location &result = WebServer::matchURI(URI, locations);
	return assertEqual("location block root match", result, locations[0]);
}

bool test_multiple_matches()
{
	vector<Location> locations;
	locations.push_back(Location("/"));
	locations.push_back(Location("/a"));
	locations.push_back(Location("/a/b"));
	const string URI = "/a/b/c";
	const Location &result = WebServer::matchURI(URI, locations);
	return assertEqual("location block multiple matches, longest prefix", result, locations[2]);
}
