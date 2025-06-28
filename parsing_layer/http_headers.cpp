#include "http_headers.hpp"
#include <sstream>
#include "utils.hpp"

/**
 * http_headers - Constructor for http_headers class.
 */
http_headers::http_headers( void ) {};

// Methods

/**
 * clearHeaders - Clear all headers in the http_headers object.
 * 
 * This method clears the internal map of headers, effectively
 * resetting the state of the http_headers object.
 */
void	http_headers::clearHeaders( void ) {
	this->_headers.clear();
};

/**
 * loadHeaders - Load headers from a string into the http_headers object.
 * 
 * This method parses the input string, splits it by newlines,
 * and populates the internal map of headers with key-value pairs.
 * 
 * @param headers: the string containing headers in the format "key: value".
 */
void	http_headers::loadHeaders( std::string const& headers ) {
	if (headers.empty()) {
		return ;
	}
	
	std::istringstream stream(headers);
	std::string line;

	while (std::getline(stream, line, '\n')) {
		if (line.empty()) continue; // skip empty lines
		size_t pos = line.find(':');
		if (pos == std::string::npos) continue; // skip lines without ':'
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);
		if (!key.empty() && !value.empty()) {
			this->_headers[key] = value;
		}
	}
}

// getters

/**
 * getHeaders - Get the internal map of headers.
 * 
 * This method returns a constant reference to the internal map
 * of headers, allowing read-only access to the headers.
 * 
 * @return: const reference to the map of headers.
 */
const std::map<std::string, std::string>& http_headers::getHeaders() const {
	return this->_headers;
}

/**
 * operator<< - Overloaded output operator for http_headers.
 * 
 * This method allows printing the headers to an output stream
 * in the format "key: value\r\n".
 * 
 * @param os: the output stream to write to.
 * @param headers: the http_headers object containing headers.
 * 
 * @return: reference to the output stream.
 */
std::ostream& operator<<(std::ostream& os, std::map<std::string, std::string> const &headers) {
	for (
		std::map<std::string, std::string>::const_iterator it = headers.begin();
		it != headers.end();
		++it
	) {
		os << it->first << ":" << it->second << "\r\n";
	}
	return os;
}
