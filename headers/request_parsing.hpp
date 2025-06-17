#pragma once

#include <iostream>

// class request_line {
// private:
// 	std::string _method; // e.g., GET, POST, PUT, DELETE
// 	std::string _uri; // e.g., /index.html
// 	std::string _http_version; // e.g., HTTP/1.1

// public:
// 	request_line(std::string const start_line);
// };


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
