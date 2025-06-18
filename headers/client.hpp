#pragma once

#include <iostream>
#include "request_parsing.hpp"
#include "http_headers.hpp"

/**
 * client - all data related to a connection goes here
 */
class client {
private:
	std::string		_request;
	int				_sock;
	static int		_num_conx; // track id's count.
	static int		_connections; // open connections count.
	int				_id; // client id, unique for each connection.

	// parser
	request_parsing	_request_parser;
	
	// no copy, no instantiation without client socket.
	client( void );
	client( client &copy );
	client& operator=( client& copy );
public:	
	client( int sock );
	~client( void );

	// methods
	void	reqAppend( char *req_buffer, ssize_t size );
	void	clearReq( void );
	
	// getters
	std::string const&	getRequest( void );
	int					getSock( void );
	int					getNumConx( void );
	int					getClientId( void );
	request_parsing&	getRequestParser( void ) { return _request_parser; }

	// setters
	void				decrementNumConx() { _connections--; };
};
