#include "request_parsing.hpp"
#include "utils.hpp"

/**
 * request_parsing - Constructor for request_parsing class.
 */
request_parsing::request_parsing(void) {
	this->_parser_status = NOT_STARTED;
	this->_body = false;
};

// methods

/**
 * parse - Parse the HTTP request string.
 * 
 * This method processes the input request string, extracting the request line,
 * headers, and body content if present. It updates the parser status accordingly.
 * 
 * @param request: the HTTP request string to be parsed.
 */
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
		// dont support chuncked transfer encoding.
		if (this->_headers.getHeaders().find("Transfer-Encoding") != this->_headers.getHeaders().end()) {
			// to handle properly for an error response.
			INFO_LOGS && std::cout << "Transfer-Encoding: chuncked" << std::endl;
			return (this->_parser_status = READING_ERROR, void());
		}
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
	this->_start_line.clearUrl();
	this->_start_line.setMethod("");
	this->_headers.clearHeaders();
	this->_bodyContent.clear();
};

// getters

/**
 * getStatus - Get the current parsing status.
 * 
 * This method returns the current status of the request parser,
 * indicating whether it is in progress, completed, or has encountered an error.
 * 
 * @return: the current parsing status as an enum value.
 */
parsing_status request_parsing::getStatus() const {
	return this->_parser_status;
}

/**
 * getRequestLine - Get the parsed request line.
 * 
 * This method returns a constant reference to the request_line object,
 * which contains the HTTP method, URI, and HTTP version extracted from the request.
 * 
 * @return: constant reference to the request_line object.
 */
request_line const& request_parsing::getRequestLine() const {
	return this->_start_line;
}

/**
 * getHeadersMap - Get the parsed HTTP headers.
 * 
 * This method returns a constant reference to the http_headers object,
 * which contains the headers parsed from the request.
 * 
 * @return: constant reference to the http_headers object.
 */
http_headers const& request_parsing::getHeadersMap() const {
	return this->_headers;
}

/**
 * getBodyContent - Get the body content of the request.
 * 
 * This method returns a constant reference to the body content string,
 * which contains the data sent in the body of the request, if any.
 * 
 * @return: constant reference to the body content string.
 */
std::string const&	request_parsing::getBodyContent() const {
	return this->_bodyContent;
}
