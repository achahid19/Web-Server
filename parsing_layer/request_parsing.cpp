#include "request_parsing.hpp"

// constructor by parameter
request_parsing::request_parsing(std::string const& request) : _request(request) {
	// No additional initialization needed
}

// getters
std::string request_parsing::getRequest() const {
	return _request;
}
