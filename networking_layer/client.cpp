#include "client.hpp"

int	client::_num_conx = 0;
int	client::_connections = 0;

// constructor
client::client( int sock ) {
	this->_id = ++this->_num_conx;
	this->_sock = sock;
	this->_connections++;
}

client::~client( void ) {
	this->_request.clear();
};

// Methods
void	client::reqAppend( char *req_buff, size_t size ) {
	this->_request.append(req_buff, size);
}

void	client::clearReq( void ) {
	this->_request.clear();
}

// getters
std::string const&	client::getRequest( void ) {
	return this->_request;
}

int	client::getSock( void ) {
	return this->_sock;
}

int	client::getNumConx( void ) {
	return this->_connections;
}

int	client::getClientId( void ) {
	return this->_id;
}
