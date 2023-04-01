#ifndef WEB_SERVER_UTILS_UTILS_HPP_
#define WEB_SERVER_UTILS_UTILS_HPP_

#include <string>
#include <sstream>

std::string	to_string(size_t num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

std::string	to_string(int num)
{
	std::stringstream ss;
	ss << num;
	return ss.str();
}

#endif //WEB_SERVER_UTILS_UTILS_HPP_
