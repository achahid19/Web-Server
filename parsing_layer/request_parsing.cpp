#include "request_parsing.hpp"

// constructor
request_parsing::request_parsing(void) {};

// methods
void request_parsing::parse(const std::string& request) {
	if (request.empty()) {
		return (this->_status = parsing_status::NOT_STARTED, void());
	}

	// Parse the request line
	size_t pos = request.find("\r\n");
	if (pos == std::string::npos) {
		return (this->_status = parsing_status::REQUEST_LINE, void());
	}

	std::string start_line = request.substr(0, pos);
	_start_line.loadStartLine(start_line);

	// Check if there are headers
	size_t headers_start = pos + 2; // Skip "\r\n"
	size_t headers_end = request.find("\r\n\r\n", headers_start);
	if (headers_end == std::string::npos) {
		return (this->_status = parsing_status::HEADERS, void());
	}

	this->_status = parsing_status::COMPLETED;
}
