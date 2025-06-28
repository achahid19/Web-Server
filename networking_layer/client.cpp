#include "client.hpp"

int	client::_num_conx = 0;
int	client::_connections = 0;

/**
 * client - Constructor for client class.
 * 
 * @param sock: the socket file descriptor for the client connection.
 */
client::client( int sock ) {
	this->_id = ++this->_num_conx;
	this->_sock = sock;
	this->_connections++;
}

/**
 * ~client - Destructor for client class.
 * 
 * Cleans up Client resources.
 */
client::~client( void ) {
	this->_request.clear();
};

// Methods

/** reqAppend - Append data to the client's request buffer.
 * 
 * @param req_buff: pointer to the request buffer.
 * @param size: size of the request buffer.
 */
void	client::reqAppend( char *req_buff, size_t size ) {
	this->_request.append(req_buff, size);
}

/**
 * clearReq - Clear the client's request buffer.
 * 
 * This method clears the request string, effectively resetting
 * the client's request state.
 */
void	client::clearReq( void ) {
	this->_request.clear();
}

// getters

/**
 * getRequest - Get the client's request string.
 * 
 * @return: const reference to the request string.
 */
std::string const&	client::getRequest( void ) {
	return this->_request;
}

/**
 * getSock - Get the client's socket file descriptor.
 * 
 * @return: the socket file descriptor.
 */
int	client::getSock( void ) {
	return this->_sock;
}

/**
 * getNumConx - Get the number of open connections.
 * 
 * @return: the number of open connections.
 */
int	client::getNumConx( void ) {
	return this->_connections;
}

/**
 * getClientId - Get the client's unique ID.
 * 
 * @return: the unique ID of the client.
 */
int	client::getClientId( void ) {
	return this->_id;
}
