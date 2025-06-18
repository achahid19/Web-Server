#pragma once

#include <iostream>
#include "request_line.hpp"
#include <map>
#include "http_headers.hpp"

enum class parsing_status {
		NOT_STARTED,
		REQUEST_LINE,
		HEADERS,
		BODY,
		IN_PROGRESS,
		COMPLETED,
		ERROR
};

class http_headers;
class request_parsing;

class request_parsing {
private:
	// states
	bool			_body = false;
	parsing_status 	_status = parsing_status::NOT_STARTED;
	
	// request structure
	request_line 	_start_line;
	http_headers	_headers;
	
	// Private copy constructor and assignment operator to prevent copying
	request_parsing(const request_parsing& copy);
	request_parsing& operator=(const request_parsing& copy);
	
public:
	// Constructor by parameter
	request_parsing( void );

	// methods
	void	parse(const std::string& request);
	void	resetParser( void );

	// getters
	std::string getRequest() const;
	parsing_status getStatus() const { return _status; };
	request_line const& getStartLine() const { return _start_line; };
};
