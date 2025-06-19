#include "request_parsing.hpp"
#include "utils.hpp"

// constructor
request_parsing::request_parsing(void) {
	this->_parser_status = NOT_STARTED;
	this->_body = false;
};

// methods
void request_parsing::parse(const std::string& request) {
	if (request.empty()) {
		return (this->_parser_status = NOT_STARTED, void());
	}

	size_t pos = request.find("\r\n");
	size_t headers_start = pos + 2;
	size_t headers_end;
	
	if (
		this->_parser_status == NOT_STARTED
		|| this->_parser_status == REQUEST_LINE
	)
	{
		// Parse the request line
		if (pos == std::string::npos) {
			return (this->_parser_status = REQUEST_LINE, void());
		}

		std::string start_line = request.substr(0, pos);
		_start_line.loadRequestLine(start_line);
		this->_parser_status = HEADERS;
	}

	if (this->_parser_status == HEADERS) {
		// Check if there are headers
		headers_end = request.find("\r\n\r\n", headers_start);
		if (headers_end == std::string::npos) {
			return ;
		}
		// load headers map<string key, string value>
		std::string headers = request.substr(headers_start, headers_end);
		this->_headers.loadHeaders(headers);
		if (
			this->_start_line.getMethod() == "GET"
			|| this->_start_line.getMethod() == "DELETE"
		) {
			return (this->_parser_status = COMPLETED, void());
		}
		
		else if (this->_start_line.getMethod() == "POST") {
			INFO_LOGS && std::cout << "Expecting a body" << std::endl;
			this->_body = true;
			this->_parser_status = BODY;
		}
	}
	
	if (this->_body) {
		/**
		 * IMPLEMENT CHUNCKS MECHANISM LATER.
		 */
		size_t body_start = headers_end + 4;
		size_t body_end = request.find("\r\n", body_start);
		if (body_end == std::string::npos) {
			return ;
		}
		this->_bodyContent = request.substr(body_start, body_end);
	}

	this->_parser_status = COMPLETED;
}

void	request_parsing::resetParser( void ) {
	this->_body = false;
	this->_parser_status = NOT_STARTED;
	this->_start_line.setHttpVersion("");
	this->_start_line.setUri("");
	this->_start_line.setMethod("");
	this->_headers.clearHeaders();
};

// getters
parsing_status request_parsing::getStatus() const {
	return this->_parser_status;
}

request_line const& request_parsing::getRequestLine() const {
	return this->_start_line;
}

http_headers const& request_parsing::getHeadersMap() const {
	return this->_headers;
}

std::string const&	request_parsing::getBodyContent() const {
	return this->_bodyContent;
}
