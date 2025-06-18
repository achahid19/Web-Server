#pragma once

#include <iostream>
#include "request_line.hpp"

enum class parsing_status {
		NOT_STARTED,
		REQUEST_LINE,
		HEADERS,
		BODY,
		IN_PROGRESS,
		COMPLETED,
		ERROR
};

class request_parsing;

class request_parsing {
private:
	bool body = false;
	parsing_status _status = parsing_status::NOT_STARTED;
	request_line _start_line;
	
	// Private copy constructor and assignment operator to prevent copying
	request_parsing(const request_parsing& copy);
	request_parsing& operator=(const request_parsing& copy);
	
public:
	// Constructor by parameter
	request_parsing( void );

	// methods
	void parse(const std::string& request);

	// getters
	std::string getRequest() const;
	parsing_status getStatus() const { return _status; };
	request_line const& getStartLine() const { return _start_line; };
};
