#ifndef FORCE_TOSTRING_H
#define FORCE_TOSTRING_H
#include <string>

template <class T>
class type{
public:
	T value;
	std::wstring toString()
	{
		return std::to_wstring(value);
	}
	std::string toStringA()
	{
		return std::to_string(value);
	}
};

#endif