#pragma once

#include <iostream>
#include <map>

class http_headers {
private:
	std::map<std::string, std::string> _headers;

public:
	// constructor
	http_headers( void );

	// Methods
	void	clearHeaders( void );
	void	loadHeaders( std::string const &headers );
};
