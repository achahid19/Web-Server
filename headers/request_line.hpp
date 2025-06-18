#pragma once

#include <iostream>

class request_line {
private:
	std::string _method;
 	std::string _uri;
 	std::string _http_version;

public:
 	request_line( void );

	// Methods
	void loadStartLine(const std::string& start_line);

	// setters
	void setMethod(const std::string& method);
	void setUri(const std::string& uri);
	void setHttpVersion(const std::string& http_version);

	// getters
	std::string getMethod() const;
	std::string getUri() const;
	std::string getHttpVersion() const;
};

std::ostream& operator<<(std::ostream& os, const request_line& rl);
