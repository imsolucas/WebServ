# include <sstream>

# include "utils.hpp"

using std::string;
using std::istringstream;
using std::vector;
using std::getline;

vector<string> utils::split(const string &str, char delimiter)
{
	istringstream	iss(str);
	string			token;
	vector<string>	v;

	while (getline(iss, token, delimiter))
		v.push_back(token);

	return v;
}
