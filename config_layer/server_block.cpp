#include "server_block.hpp"
#include "utils.hpp"

/**
 * TODO list -
 * - set a proper exception handling class for server_block.n
 * 
 */

// constructor
server_block::server_block( void ) {};

// destructor
server_block::~server_block( void ) {
	this->_directives.clear();
}

// Setters;
void	server_block::setPort( const std::string &port ) {
	this->_validDirectiveValue(port);
	this->_directives.insert(std::make_pair(
		"port",
		ft_trim_spaces(port.substr(0, port.find(';')))
	));
}

void	server_block::setHost( const std::string & host ) {
	this->_validDirectiveValue(host);
	this->_directives.insert(std::make_pair(
		"host",
		ft_trim_spaces(host.substr(0, host.find(';')))
	));
}

void	server_block::setServerName( const std::string &serverName ) {
	this->_validDirectiveValue(serverName);
	this->_directives.insert(std::make_pair(
		"server_name",
		ft_trim_spaces(serverName.substr(0, serverName.find(';')))
	));
}

void	server_block::setRoot( const std::string &root ) {
	this->_validDirectiveValue(root);
	this->_directives.insert(std::make_pair(
		"root",
		ft_trim_spaces(root.substr(0, root.find(';')))
	));
}

// Getters
const std::string&	server_block::getPort( void ) const {
	if (this->_directives.find("port") == this->_directives.end())
		throw config_error("Port directive not found in server block");
	return this->_directives.at("port");
}

const std::string&	server_block::getHost( void ) const {
	if (this->_directives.find("host") == this->_directives.end())
		throw config_error("Host directive not found in server block");
	return this->_directives.at("host");
}

const std::string&	server_block::getServerName( void ) const {
	if (this->_directives.find("server_name") == this->_directives.end())
		throw config_error("Server name directive not found in server block");
	return this->_directives.at("server_name");
}

const std::string&	server_block::getRoot( void ) const {
	if (this->_directives.find("root") == this->_directives.end())
		throw config_error("Root directive not found in server block");
	return this->_directives.at("root");
}

// safe getters
const std::string	server_block::get_port_safe( void ) const {
	return this->_directives.count("port") ? this->_directives.at("port") : "";
}

const std::string	server_block::get_host_safe( void ) const {
	return this->_directives.count("host") ? this->_directives.at("host") : "";
}

const std::string	server_block::get_server_name_safe( void ) const {
	return this->_directives.count("server_name") ? this->_directives.at("server_name") : "";
}

const std::string	server_block::get_root_safe( void ) const {
	return this->_directives.count("root") ? this->_directives.at("root") : "";
}


/**
 * General Directive's value check (for server config block).
 */
void server_block::_validDirectiveValue( const std::string &value ) {
	if (value[value.length() - 1] != ';')
		throw config_error(
			"Directive's value doesn't end with semicolon"
	);
	if (value.empty())
		throw config_error("Directive's value is empty");
	if (value[0] == ';')
		throw config_error("Directive's value cannot start with semicolon");
}

// exception handling
server_block::config_error::config_error( const std::string &msg ) : _msg(msg) {};

const char* server_block::config_error::what() const throw() {
	return _msg.c_str();
}