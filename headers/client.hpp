#pragma once

#include <iostream>

/**
 * client - all data related to a connection goes here
 */
class client {
private:
	std::string		_request;
	int				_sock;
	static int		_num_conx; // track number of all connections.
	int				_id; // track the number of current client.

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
};
