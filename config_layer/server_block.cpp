#include "server_block.hpp"
#include "utils.hpp"

/**
 * TODO list -
 * - set a proper exception handling class for server_block.n
 * 
 */

static void validDirectiveValue( const std::string value );

// constructor
server_block::server_block( void ) {};

// destructor
server_block::~server_block( void ) {
	this->_directives.clear();
}

// Setters;
void	server_block::setPort( const std::string &port ) {
	::validDirectiveValue(port);
	this->_directives.insert(std::make_pair("port", port));
}

void	server_block::setHost( const std::string & host ) {
	::validDirectiveValue(host);
	this->_directives.insert(std::make_pair("host", host));
}

void	server_block::setServerName( const std::string &serverName ) {
	::validDirectiveValue(serverName);
	this->_directives.insert(std::make_pair("server_name", serverName));
}

void	server_block::setRoot( const std::string &root ) {
	::validDirectiveValue(root);
	this->_directives.insert(std::make_pair("root", root));
}

// Getters
const std::string&	server_block::getPort( void ) const {
	if (this->_directives.find("port") == this->_directives.end())
		throw std::out_of_range("Port directive not found in server block");
	return this->_directives.at("port");
}

const std::string&	server_block::getHost( void ) const {
	if (this->_directives.find("host") == this->_directives.end())
		throw std::out_of_range("Host directive not found in server block");
	return this->_directives.at("host");
}

const std::string&	server_block::getServerName( void ) const {
	if (this->_directives.find("server_name") == this->_directives.end())
		throw std::out_of_range("Server name directive not found in server block");
	return this->_directives.at("server_name");
}

const std::string&	server_block::getRoot( void ) const {
	if (this->_directives.find("root") == this->_directives.end())
		throw std::out_of_range("Root directive not found in server block");
	if (this->_directives.at("root").empty())
		throw std::out_of_range("Root directive is empty in server block");
	return this->_directives.at("root");
}

/**
 * General Directive's value check (for server config block).
 */
static void validDirectiveValue( const std::string value ) {
	if (value[value.length() - 1] != ';')
		throw std::runtime_error(
			"Directive's value doesn't end with semicolon"
	);
	if (value.empty())
		throw std::runtime_error("Directive's value is empty");
	if (value[0] == ';')
		throw std::runtime_error("Directive's value cannot start with semicolon");
}