#include "http_headers.hpp"
#include <sstream>
#include "utils.hpp"

// Constructor
http_headers::http_headers( void ) {};

// Methods
void	http_headers::clearHeaders( void ) {
	this->_headers.clear();
};

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
const std::map<std::string, std::string>& http_headers::getHeaders() const {
	return this->_headers;
}

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
