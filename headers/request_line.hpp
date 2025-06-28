#pragma once

#include <iostream>
#include "url.hpp"

class url;

class request_line {
private:
	std::string	_method;
 	url			_url;
 	std::string	_http_version;

public:
	request_line( void );

	// Methods
	void loadRequestLine(const std::string& start_line);

	// setters
	void setMethod(const std::string& method);
	void setHttpVersion(const std::string& http_version);

	// getters
	std::string getMethod() const;
	std::string getUri() const;
	std::string getHttpVersion() const;

	// clear url
	void clearUrl( void );
};

std::ostream& operator<<(std::ostream& os, const request_line& rl);
