#pragma once

#include <iostream>
#include <map>

class server_block {
private:
	std::map<std::string, std::string> _directives;
	// location structure goes here, for later implementation

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
	void setIndex(const std::string &index);
	void setClientMaxBodySize(const std::string &size);

	void	setAsDefault( void );

	// getters
	// those methods will throw an exception if the directive is not found
	const std::string& getPort() const;
	const std::string& getHost() const;
	const std::string& getServerName() const;
	const std::string& getRoot() const;
	const std::string& getIndex() const;
	const std::string& getClientMaxBodySize() const;
	
	// those methods wont throw an exception if the directive is not found
	const std::string get_port_safe() const;
	const std::string get_host_safe() const;
	const std::string get_server_name_safe() const;
	const std::string get_root_safe() const;
	const std::string get_index_safe() const;
	const std::string get_client_max_body_size_safe() const;

	bool	isDefault() const;

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
