#pragma once

#include <iostream>
#include <sstream>

class request_parsing {
private:
	std::string const _request;

	// Private copy constructor and assignment operator to prevent copying
	request_parsing(const request_parsing& copy);
	request_parsing& operator=(const request_parsing& copy);
	request_parsing( void );

public:
	// Constructor by parameter
	request_parsing(std::string const& request);

	// getters
	std::string getRequest() const;
};

// turn an int to a string in a c++98 compliant way
std::string	ft_to_string(int value);
