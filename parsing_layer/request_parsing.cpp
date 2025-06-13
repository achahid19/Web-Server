#include "request_parsing.hpp"

// constructor by parameter
request_parsing::request_parsing(std::string const& request) : _request(request) {
	// No additional initialization needed
}

// getters
std::string request_parsing::getRequest() const {
	return _request;
}

// utility functions

std::string ft_to_string(int value) {
	// c++98 compliant

	std::stringstream ss;
	ss << value;
	return ss.str();
}
