#include "url.hpp"

// Methods
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
std::string url::getPathToResource() const {
	return this->_path_to_resource;
}

std::string url::getFullUrl() const {
	if (this->_query_string.empty()) {
		return this->_path_to_resource;
	}
	return this->_path_to_resource + "?" + this->_query_string;
}

std::string url::getQueryString() const {
	return this->_query_string;
}
