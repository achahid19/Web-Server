#pragma once

#include <iostream>

class server_block {
private:
	std::string		_port;
	std::string		_host;
	std::string		_server_name;
	std::string		_root;
	unsigned int	_client_max_body_size;
	std::string		_index;
	// bool			_autoindex;

public:
	server_block( void ) {};
	~server_block( void ) {};

	// setters
	void setPort(const std::string &port) { _port = port; }
	void setHost(const std::string &host) { _host = host; }
	void setServerName(const std::string &server_name) { _server_name = server_name; }
	void setRoot(const std::string &root) { _root = root; }
	void setClientMaxBodySize(unsigned int size) { _client_max_body_size = size; }
	void setIndex(const std::string &index) { _index = index; }

	// getters
	std::string getPort() const { return _port; }
	std::string getHost() const { return _host; }
	std::string getServerName() const { return _server_name; }
	std::string getRoot() const { return _root; }
	unsigned int getClientMaxBodySize() const { return _client_max_body_size; }
	std::string getIndex() const { return _index; }
};
