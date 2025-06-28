#pragma once

#include <iostream>

class url {
private:
	std::string	_path_to_resource;
	std::string	_query_string;

public:
	url( void ) {};
	~url( void ) {};

	// Methods
	void loadUrl(const std::string& uri);

	// getters
	std::string getPathToResource() const;
	std::string getQueryString() const;
	std::string getFullUrl() const;

	// clear
	void clearUrl( void ) {
		_path_to_resource.clear();
		_query_string.clear();
	}
};
