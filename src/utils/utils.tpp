# include "utils.hpp"

using std::vector;

template <typename T>
bool utils::contains(const T &element, const vector<T> &vec)
{
	for (typename vector<T>::const_iterator it = vec.begin();
		it != vec.end(); ++it)
	{
		if (*it == element) return true;
	}
	return false;
}
