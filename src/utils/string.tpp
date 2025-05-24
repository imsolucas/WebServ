# include <sstream>

# include "utils.hpp"

using std::string;
using std::ostringstream;

template<typename T>
string utils::toString(const T &any)
{
	ostringstream oss;

	oss << any;

	return oss.str();
}
