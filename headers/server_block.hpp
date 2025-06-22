#pragma once

#include <iostream>
#include <map>

class server_block {
private:
	std::map<std::string, std::string> _directives; // directives map

	// std::string		_port;
	// std::string		_host;
	// std::string		_server_name;
	// std::string		_root;
	// unsigned int	_client_max_body_size;
	// std::string		_index;
	// bool			_autoindex;

public:
	server_block( void );
	~server_block( void );

	// setters
	void setPort(const std::string &port);
	void setHost(const std::string &host);
	void setServerName(const std::string &server_name);
	void setRoot(const std::string &root);
	// void setClientMaxBodySize(unsigned int size);
	// void setIndex(const std::string &index);

	// getters
	const std::string& getPort() const;
	const std::string& getHost() const;
	const std::string& getServerName() const;
	const std::string& getRoot() const;
	// unsigned int getClientMaxBodySize() const;
	// std::string getIndex() const;
};
