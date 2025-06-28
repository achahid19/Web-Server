#include "request_line.hpp"
#include "utils.hpp"

/**
 * request_line - Constructor for request_line class.
 */
request_line::request_line(void) {};

// methods

/**
 * loadRequestLine - Load the request line from a string.
 * 
 * This method parses the input string, extracting the HTTP method,
 * URI, and HTTP version, and populates the request_line object.
 * 
 * @param start_line: the request line in the format "METHOD URI HTTP/VERSION".
 */
void request_line::loadRequestLine(const std::string& start_line) {
	size_t method_end = start_line.find(' ');
	if (method_end == std::string::npos) {
		return ;
	}
	this->_method = start_line.substr(0, method_end);

	size_t uri_start = method_end + 1;
	size_t uri_end = start_line.find(' ', uri_start);
	if (uri_end == std::string::npos) {
		return ;
	}
	this->_url.loadUrl(
		start_line.substr(uri_start, uri_end - uri_start)
	);

	URL_LOGS && std::cout << "URI: " << this->_url.getFullUrl() << std::endl;
	URL_LOGS && std::cout << "Path: " << this->_url.getPathToResource() << std::endl;
	URL_LOGS && std::cout << "Query: " << this->_url.getQueryString() << std::endl;

	size_t version_start = uri_end + 1;
	if (version_start >= start_line.size()) {
		return ;
	}
	this->_http_version = start_line.substr(version_start);
};

// setters

/**
 * setMethod - Set the HTTP method for the request line.
 * 
 * @param method: the HTTP method (e.g., GET, POST, etc.).
 */
void request_line::setMethod(const std::string& method) {
	_method = method;
}

/**
 * setHttpVersion - Set the HTTP version for the request line.
 * 
 * @param http_version: the HTTP version (e.g., HTTP/1.1).
 */
void request_line::setHttpVersion(const std::string& http_version) {
	_http_version = http_version;
}	

// getters

/**
 * getMethod - Get the HTTP method for the request line.
 * 
 * @return: the HTTP method as a string.
 */
std::string request_line::getMethod() const { 
	return _method;
}

/**
 * getHttpVersion - Get the HTTP version for the request line.
 * 
 * @return: the HTTP version as a string.
 */
std::string request_line::getHttpVersion() const { 
	return _http_version;
}

/**
 * getUri - Get the full URI for the request line.
 * 
 * @return: the full URI as a string.
 */
std::string request_line::getUri() const {
	return _url.getFullUrl();
}

/**
 * operator<< - Overloaded output operator for request_line.
 * 
 * This method allows printing the request line to an output stream
 * in a human-readable format.
 * 
 * @param os: the output stream to write to.
 * @param rl: the request_line object to print.
 * 
 * @return: reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, const request_line& rl) {
	os << "Method: " << rl.getMethod() << "\n"
	   << "URI: " << rl.getUri() << "\n"
	   << "HTTP Version: " << rl.getHttpVersion();
	return os;
};

/**
 * clearUrl - Clear the URL in the request line.
 * 
 * This method clears the internal URL object, effectively resetting
 * the URL state of the request_line object.
 */
void request_line::clearUrl(void) {
	_url.clearUrl();
}
