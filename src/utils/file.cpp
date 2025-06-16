# include <fstream>

# include "utils.hpp"

using std::runtime_error;
using std::ifstream;
using std::string;
using std::getline;

string utils::readFile(const string &path)
{
	ifstream file(path.c_str());
	if (!file.is_open())
		throw runtime_error("Failed to open the file.");

	string content;
	string line;
	while (getline(file, line))
		content += line + "\n";

	file.close();

	return content;
}
