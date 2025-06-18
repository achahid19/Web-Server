#include "request_parsing.hpp"
#include "utils.hpp"

// constructor
request_parsing::request_parsing(void) {};

// methods
void request_parsing::parse(const std::string& request) {
	if (request.empty()) {
		return (this->_status = parsing_status::NOT_STARTED, void());
	}

	size_t pos = request.find("\r\n");
	size_t headers_start = pos + 2;
	size_t headers_end;
	
	if (
		this->_status == parsing_status::NOT_STARTED
		|| this->_status == parsing_status::REQUEST_LINE
	)
	{
		// Parse the request line
		if (pos == std::string::npos) {
			return (this->_status = parsing_status::REQUEST_LINE, void());
		}

		std::string start_line = request.substr(0, pos);
		_start_line.loadStartLine(start_line);
		this->_status = parsing_status::HEADERS;
	}

	if (this->_status == parsing_status::HEADERS) {
		// Check if there are headers
		headers_end = request.find("\r\n\r\n", headers_start);
		if (headers_end == std::string::npos) {
			return ;
		}

		if (
			this->_start_line.getMethod() == "GET"
			|| this->_start_line.getMethod() == "DELETE"
		) {
			return (this->_status = parsing_status::COMPLETED, void());
		}
		
		else {
			INFO_LOGS && std::cout << "Expecting a body" << std::endl;
			this->_body = true;
			this->_status = parsing_status::BODY;
		}
	}
	
	if (this->_body) {
		size_t body_start = headers_end + 4;
		size_t body_end = request.find("\r\n", body_start);
		if (body_end == std::string::npos) {
			return ;
		}
	}

	this->_status = parsing_status::COMPLETED;
}

void	request_parsing::resetParser( void ) {
	this->_body = false;
	this->_status = parsing_status::NOT_STARTED;
	this->_start_line.setHttpVersion("");
	this->_start_line.setUri("");
	this->_start_line.setMethod("");
};
