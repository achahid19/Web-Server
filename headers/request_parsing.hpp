#pragma once

#include <iostream>

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
