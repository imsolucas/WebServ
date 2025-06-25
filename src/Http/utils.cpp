# include <sys/stat.h>

# include "Http.hpp"

using std::map;
using std::vector;
using std::string;

PathType getPathType(const string &path)
{
	struct stat pathStat;
	if (stat(path.c_str(), &pathStat) == -1)
		return NOT_EXIST;
	if (S_ISREG(pathStat.st_mode))
		return R_FILE;
	if (S_ISDIR(pathStat.st_mode))
		return DIRECTORY;
	return OTHER;
}

string getContentType(const string &file)
{
	size_t dotPos = file.find_last_of('.');
	if (dotPos == string::npos)
		return "application/octet-stream";

	string extension = file.substr(dotPos);
	map<string, string>::const_iterator it = Http::mimeType.find(extension);
	if (it != Http::mimeType.end())
		return it->second;

	return "application/octet-stream";
}


bool isCGI(const string &file)
{
	vector<string> extensions;
	extensions.push_back(".cgi");
	extensions.push_back(".py");
	extensions.push_back(".php");
	extensions.push_back(".go");
	extensions.push_back(".c");
	extensions.push_back(".sh");

	size_t dotPos = file.find_last_of('.');
	if (dotPos == string::npos) return false;

	string extension = file.substr(dotPos);
	for (vector<string>::const_iterator it = extensions.begin();
		it != extensions.end(); ++it)
		if (extension == *it) return true;

	return false;
}
