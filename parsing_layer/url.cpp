#include "url.hpp"

// Methods

/**
 * loadUrl - Load the URL from a string.
 * 
 * This method parses the input string, extracting the path to the resource
 * and the query string if present.
 * 
 * @param uri: the URL string to be parsed, which may include a query string.
 * 
 * @return: void.
 */
void url::loadUrl(const std::string& uri) {
	size_t path_end = uri.find('?');

	if (path_end == std::string::npos) {
		this->_path_to_resource = uri;
	} else {
		this->_path_to_resource = uri.substr(0, path_end);
		this->_query_string = uri.substr(path_end + 1);
	}
}

// getters

/**
 * getPathToResource - Get the path to the resource from the URL.
 * 
 * This method returns the path to the resource part of the URL,
 * excluding any query string.
 * 
 * @return: the path to the resource as a string.
 */
std::string url::getPathToResource() const {
	return this->_path_to_resource;
}

/**
 * getFullUrl - Get the full URL including the query string.
 * 
 * This method constructs the full URL by combining the path to the resource
 * and the query string, if present.
 * 
 * @return: the full URL as a string.
 */
std::string url::getFullUrl() const {
	if (this->_query_string.empty()) {
		return this->_path_to_resource;
	}
	return this->_path_to_resource + "?" + this->_query_string;
}

/**
 * getQueryString - Get the query string from the URL.
 * 
 * This method returns the query string part of the URL,
 * which is the part after the '?' character.
 * 
 * @return: the query string as a string.
 */
std::string url::getQueryString() const {
	return this->_query_string;
}
