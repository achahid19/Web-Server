#include "utils.hpp"

/**
 * ft_to_string - convert an integer to a string c++98 compliant.
 * @param value: integer to convert.
 * 
 * Return: string representation of the integer.
 */
std::string	ft_to_string( int value ) {
	std::stringstream ss;

	ss << value;
	return ss.str();
}

/**
 * ft_trim_spaces - trim leading and trailing spaces from a string.
 * @param str: the string to trim.
 * 
 * Return: trimmed string.
 */
std::string	ft_trim_spaces( const std::string &str ) {
	size_t start = str.find_first_not_of(" \t\n\r\f\v");
	if (start == std::string::npos) {
		return ""; // string is all spaces
	}
	size_t end = str.find_last_not_of(" \t\n\r\f\v");
	return str.substr(start, end - start + 1);
}
