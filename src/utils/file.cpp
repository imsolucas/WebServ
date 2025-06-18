# include <fstream>
# include <dirent.h>

# include "utils.hpp"

using std::runtime_error;
using std::vector;
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

// return directory entries
vector<string> utils::readDirectory(const string &path)
{
	vector<string> entries;

	DIR *dir = opendir(path.c_str());
	if (dir == NULL)
		return entries;

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		string name = entry->d_name;
		if (name == "." || name == "..") continue;
		entries.push_back(name);
	}

	closedir(dir);

	return entries;
}
