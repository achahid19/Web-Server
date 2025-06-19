#include "request_line.hpp"

// constructor
request_line::request_line(void) {};

// methods
void request_line::loadRequestLine(const std::string& start_line) {
	size_t method_end = start_line.find(' ');
	if (method_end == std::string::npos) {
		// throw std::invalid_argument("Invalid request line: missing method");
		return ;
	}
	this->_method = start_line.substr(0, method_end);

	size_t uri_start = method_end + 1;
	size_t uri_end = start_line.find(' ', uri_start);
	if (uri_end == std::string::npos) {
		//throw std::invalid_argument("Invalid request line: missing URI");
		return ;
	}
	this->_uri = start_line.substr(uri_start, uri_end - uri_start);

	size_t version_start = uri_end + 1;
	if (version_start >= start_line.size()) {
		//throw std::invalid_argument("Invalid request line: missing HTTP version");
		return ;
	}
	this->_http_version = start_line.substr(version_start);
};

// setters
void request_line::setMethod(const std::string& method) {
	_method = method;
}
void request_line::setUri(const std::string& uri) { 
	_uri = uri;
}
void request_line::setHttpVersion(const std::string& http_version) {
	_http_version = http_version;
}	

// getters
std::string request_line::getMethod() const { 
	return _method;
}
std::string request_line::getUri() const {
	return _uri; 
}
std::string request_line::getHttpVersion() const { 
	return _http_version;
}

std::ostream& operator<<(std::ostream& os, const request_line& rl) {
	os << "Method: " << rl.getMethod() << "\n"
	   << "URI: " << rl.getUri() << "\n"
	   << "HTTP Version: " << rl.getHttpVersion();
	return os;
};
