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

	// helper methods
	void _validDirectiveValue( const std::string &value );

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
	// those methods will throw an exception if the directive is not found
	const std::string& getPort() const;
	const std::string& getHost() const;
	const std::string& getServerName() const;
	const std::string& getRoot() const;
	// unsigned int getClientMaxBodySize() const;
	// std::string getIndex() const;

	// those methods wont throw an exception if the directive is not found
	const std::string get_port_safe() const;
	const std::string get_host_safe() const;
	const std::string get_server_name_safe() const;
	const std::string get_root_safe() const;

	// exception handling
	class config_error : public std::exception {
	private:
		const std::string _msg;
	public:
		config_error( const std::string &msg );
		~config_error( void ) throw() {};
		virtual const char* what() const throw();
	};
};
