#include "utils.hpp"

/**
 * ft_to_string - convert an integer to a string c++98 compliant.
 * @value: integer to convert.
 * 
 * Return: string representation of the integer.
 */
std::string	ft_to_string( int value ) {
	std::stringstream ss;

	ss << value;
	return ss.str();
}
