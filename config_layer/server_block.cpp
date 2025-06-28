#include "server_block.hpp"
#include "utils.hpp"

/**
 * TODO list -
 * - set a proper exception handling class for server_block.n
 * 
 */

/**
 * server_block - Constructor for server_block class.
 */
server_block::server_block( void ) {};

/**
 * ~server_block - Destructor for server_block class.
 * 
 * Cleans up the directives map.
 */
server_block::~server_block( void ) {
	this->_directives.clear();
}

// Setters;

/**
 * setPort - Set the port for the server block.
 * 
 * This method validates the port value and adds it to the directives map.
 * 
 * @param port: the port number as a string, should be a valid integer.
 * 
 * Throws config_error if the port value is invalid.
 */
void	server_block::setPort( const std::string &port ) {
	this->_validDirectiveValue(port);
	this->_directives.insert(std::make_pair(
		"port",
		ft_trim_spaces(port.substr(0, port.find(';')))
	));
}

/**
 * setHost - Set the host for the server block.
 * 
 * This method validates the host value and adds it to the directives map.
 * 
 * @param host: the host name or IP address as a string, should be valid.
 * 
 * Throws config_error if the host value is invalid.
 */
void	server_block::setHost( const std::string & host ) {
	this->_validDirectiveValue(host);
	this->_directives.insert(std::make_pair(
		"host",
		ft_trim_spaces(host.substr(0, host.find(';')))
	));
}

/**
 * setServerName - Set the server name for the server block.
 * 
 * This method validates the server name value and adds it to the directives map.
 * 
 * @param serverName: the server name as a string, should be valid.
 * 
 * Throws config_error if the server name value is invalid.
 */
void	server_block::setServerName( const std::string &serverName ) {
	this->_validDirectiveValue(serverName);
	this->_directives.insert(std::make_pair(
		"server_name",
		ft_trim_spaces(serverName.substr(0, serverName.find(';')))
	));
}

/**
 * setRoot - Set the root directory for the server block.
 * 
 * This method validates the root value and adds it to the directives map.
 * 
 * @param root: the root directory path as a string, should be valid.
 * 
 * Throws config_error if the root value is invalid.
 */
void	server_block::setRoot( const std::string &root ) {
	this->_validDirectiveValue(root);
	this->_directives.insert(std::make_pair(
		"root",
		ft_trim_spaces(root.substr(0, root.find(';')))
	));
}

/**
 * setIndex - Set the index file for the server block.
 * 
 * This method validates the index value and adds it to the directives map.
 * 
 * @param index: the index file name as a string, should be valid.
 * 
 * Throws config_error if the index value is invalid.
 */
void	server_block::setIndex( const std::string &index ) {
	this->_validDirectiveValue(index);
	this->_directives.insert(std::make_pair(
		"index",
		ft_trim_spaces(index.substr(0, index.find(';')))
	));
}

/**
 * setClientMaxBodySize - Set the maximum body size for client requests.
 * 
 * This method validates the size value and adds it to the directives map.
 * 
 * @param size: the maximum body size as a string, should be a valid integer.
 * 
 * Throws config_error if the size value is invalid.
 */
void	server_block::setClientMaxBodySize( const std::string &size ) {
	this->_validDirectiveValue(size);
	this->_directives.insert(std::make_pair(
		"client_max_body_size",
		ft_trim_spaces(size.substr(0, size.find(';')))
	));
}

/**
 * setAsDefault - Set the server block as the default server block.
 * 
 * This method adds a "default" directive to the directives map,
 * indicating that this server block should be used as the default.
 */
void	server_block::setAsDefault( void ) {
	this->_directives.insert(std::make_pair("default", "true"));
}

/**
 * setErrorPage - Set the error page for the server block.
 * 
 * This method validates the error page value and adds it to the directives map.
 * 
 * @param error_page: the error page path as a string, should be valid.
 * 
 * Throws config_error if the error page value is invalid.
 */
void	server_block::setErrorPage( const std::string &error_page ) {
	this->_validDirectiveValue(error_page);
	this->_directives.insert(std::make_pair(
		"error_page",
		ft_trim_spaces(error_page.substr(0, error_page.find(';')))
	));
}

// Getters

/**
 * getPort - Get the port for the server block.
 * 
 * This method retrieves the port value from the directives map.
 * 
 * Throws config_error if the port directive is not found.
 * 
 * @return: the port as a string.
 */
const std::string&	server_block::getPort( void ) const {
	if (this->_directives.find("port") == this->_directives.end())
		throw config_error("Port directive not found in server block");
	return this->_directives.at("port");
}

/**
 * getHost - Get the host for the server block.
 * 
 * This method retrieves the host value from the directives map.
 * 
 * Throws config_error if the host directive is not found.
 * 
 * @return: the host as a string.
 */
const std::string&	server_block::getHost( void ) const {
	if (this->_directives.find("host") == this->_directives.end())
		throw config_error("Host directive not found in server block");
	return this->_directives.at("host");
}

/**
 * getServerName - Get the server name for the server block.
 * 
 * This method retrieves the server name value from the directives map.
 * 
 * Throws config_error if the server name directive is not found.
 * 
 * @return: the server name as a string.
 */
const std::string&	server_block::getServerName( void ) const {
	if (this->_directives.find("server_name") == this->_directives.end())
		throw config_error("Server name directive not found in server block");
	return this->_directives.at("server_name");
}

/**
 * getRoot - Get the root directory for the server block.
 * 
 * This method retrieves the root value from the directives map.
 * 
 * Throws config_error if the root directive is not found.
 * 
 * @return: the root directory path as a string.
 */
const std::string&	server_block::getRoot( void ) const {
	if (this->_directives.find("root") == this->_directives.end())
		throw config_error("Root directive not found in server block");
	return this->_directives.at("root");
}

/**
 * getIndex - Get the index file for the server block.
 * 
 * This method retrieves the index value from the directives map.
 * 
 * Throws config_error if the index directive is not found.
 * 
 * @return: the index file name as a string.
 */
const std::string&	server_block::getIndex( void ) const {
	if (this->_directives.find("index") == this->_directives.end())
		throw config_error("Index directive not found in server block");
	return this->_directives.at("index");
}

/**
 * getClientMaxBodySize - Get the maximum body size for client requests.
 * 
 * This method retrieves the client max body size value from the directives map.
 * 
 * Throws config_error if the client max body size directive is not found.
 * 
 * @return: the maximum body size as a string.
 */
const std::string&	server_block::getClientMaxBodySize( void ) const {
	if (this->_directives.find("client_max_body_size") == this->_directives.end())
		throw config_error("Client max body size directive not found in server block");
	return this->_directives.at("client_max_body_size");
}

/**
 * getErrorPage - Get the error page for the server block.
 * 
 * This method retrieves the error page value from the directives map.
 * 
 * Throws config_error if the error page directive is not found.
 * 
 * @return: the error page path as a string.
 */
const std::string&	server_block::getErrorPage( void ) const {
	if (this->_directives.find("error_page") == this->_directives.end())
		throw config_error("Error page directive not found in server block");
	return this->_directives.at("error_page");
}

// safe getters

/**
 * get_port_safe - Get the port for the server block safely.
 * 
 * This method retrieves the port value from the directives map,
 * returning an empty string if the directive is not found.
 * 
 * @return: the port as a string, or an empty string if not found.
 */
const std::string	server_block::get_port_safe( void ) const {
	return this->_directives.count("port") ? this->_directives.at("port") : "";
}

/**
 * get_host_safe - Get the host for the server block safely.
 * 
 * This method retrieves the host value from the directives map,
 * returning an empty string if the directive is not found.
 * 
 * @return: the host as a string, or an empty string if not found.
 */
const std::string	server_block::get_host_safe( void ) const {
	return this->_directives.count("host") ? this->_directives.at("host") : "";
}

/**
 * get_server_name_safe - Get the server name for the server block safely.
 * 
 * This method retrieves the server name value from the directives map,
 * returning an empty string if the directive is not found.
 * 
 * @return: the server name as a string, or an empty string if not found.
 */
const std::string	server_block::get_server_name_safe( void ) const {
	return this->_directives.count("server_name") ? this->_directives.at("server_name") : "";
}

/**
 * get_root_safe - Get the root directory for the server block safely.
 * 
 * This method retrieves the root value from the directives map,
 * returning an empty string if the directive is not found.
 * 
 * @return: the root directory path as a string, or an empty string if not found.
 */
const std::string	server_block::get_root_safe( void ) const {
	return this->_directives.count("root") ? this->_directives.at("root") : "";
}

/**
 * get_index_safe - Get the index file for the server block safely.
 * 
 * This method retrieves the index value from the directives map,
 * returning an empty string if the directive is not found.
 * 
 * @return: the index file name as a string, or an empty string if not found.
 */
const std::string	server_block::get_index_safe( void ) const {
	return this->_directives.count("index") ? this->_directives.at("index") : "";
}

/**
 * get_client_max_body_size_safe - Get the maximum body size for client requests safely.
 * 
 * This method retrieves the client max body size value from the directives map,
 * returning an empty string if the directive is not found.
 * 
 * @return: the maximum body size as a string, or an empty string if not found.
 */
const std::string	server_block::get_client_max_body_size_safe( void ) const {
	return (
		this->_directives.count("client_max_body_size") \
		? this->_directives.at("client_max_body_size") : ""
	);
}

/**
 * get_error_page_safe - Get the error page for the server block safely.
 * 
 * This method retrieves the error page value from the directives map,
 * returning an empty string if the directive is not found.
 * 
 * @return: the error page path as a string, or an empty string if not found.
 */
const std::string	server_block::get_error_page_safe( void ) const {
	return (
		this->_directives.count("error_page") \
		? this->_directives.at("error_page") : ""
	);
}

/**
 * isDefault - Check if the server block is set as default.
 * 
 * This method checks if the "default" directive is present in the directives map.
 * 
 * @return: true if the server block is default, false otherwise.
 */
bool	server_block::isDefault( void ) const {
	return this->_directives.count("default") > 0;
}

/**
 * _validDirectiveValue - Validate the directive value.
 * 
 * This method checks if the directive value ends with a semicolon,
 * is not empty, and does not start with a semicolon.
 * 
 * Throws config_error if any of these conditions are not met.
 * 
 * @param value: the directive value to validate.
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

/**
 * config_error - Constructor for config_error class.
 * 
 * Initializes the error message with the provided string.
 * 
 * @param msg: the error message to be stored.
 */
server_block::config_error::config_error( const std::string &msg ) : _msg(msg) {};

/**
 * ~config_error - Destructor for config_error class.
 * 
 * This destructor does not perform any specific cleanup.
 */
const char* server_block::config_error::what() const throw() {
	return _msg.c_str();
}
