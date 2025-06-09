#pragma once

#include <iostream>
#include <sys/socket.h> // socket sys call
#include <netinet/in.h> // sockaddr_in structure
#include <arpa/inet.h> // inet_addr function for host
#include <fcntl.h> // fcntl().
// #include <sys/epoll.h> not working on mac

class server {
private:
	int	_serverSocket; // fd socket listenning for new client's connections
	int	_clientSocker; // establishing fd socket for new client (keep-alive).

	// dont instantiate without param
	// no copy
	server( void );
	server( server& copy );
	server&	operator&( server& copy );

public:
	server( std::string const host, int port );
	~server( void );

	void	server_run( void );
};
