#pragma once

#include <iostream>
#include "request_line.hpp"
#include <map>
#include "http_headers.hpp"

enum parsing_status {
	NOT_STARTED,
	REQUEST_LINE,
	HEADERS,
	BODY,
	IN_PROGRESS,
	COMPLETED,
	READING_ERROR
};

// struct PostFile{
// 	int fd;
// 	int offset;
// 	size_t content_length; // 'stat' variable name
// };

class http_headers;
class request_parsing;

class request_parsing {
private:
	// states
	bool			_body;
	parsing_status 	_parser_status;
	
	// request structure
	request_line 	_start_line;
	http_headers	_headers;
	std::string		_bodyContent; // struct file, read nichan into the file.
	
	// Private copy constructor and assignment operator to prevent copying
	request_parsing(const request_parsing& copy);
	request_parsing& operator=(const request_parsing& copy);
	
public:
	// Constructor
	request_parsing( void );

	// methods
	void	parse(const std::string& request);
	void	resetParser( void );

	// getters
	parsing_status		getStatus() const;
	request_line const& getRequestLine() const;
	http_headers const& getHeadersMap() const;
	std::string const&	getBodyContent() const;
};
