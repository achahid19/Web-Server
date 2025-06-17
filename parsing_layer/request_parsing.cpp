#include "request_parsing.hpp"

// constructor by parameter
request_parsing::request_parsing(std::string const& request) : _request(request) {
	// initialization needed

	/**
	 * read the start_line into a structure,
	 * (Method, URI, HTTP version for requests)
	 * (Status code, Reason phrase for responses)
	 */
	// not request, but only first line of the request.
	//request_line start_line(this->_request); // request-line for requests, and status-line for responses
}

// getters
std::string request_parsing::getRequest() const {
	return _request;
}
