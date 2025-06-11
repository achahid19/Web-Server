#pragma once

#include <iostream>
#include <sys/socket.h> // socket sys call
#include <netinet/in.h> // sockaddr_in structure
#include <arpa/inet.h> // inet_addr function for host
#include <fcntl.h> // fcntl().
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <unistd.h> // close() function
// strlen
#include <cstring> // strlen() function

#define MAX_EVENTS 5 // max number of events to handle in epoll in one go

class server {
private:
	int	_serverSocket; // fd socket listenning for new client's connections
	int	_clientSocker; // establishing fd socket for new client (keep-alive).

	// epoll attributes
	int	_epoll_fd; // interest list, handle to epoll instance
	// what if we have more than max events? # TO CHECK
	std::vector<struct epoll_event> _events; // events list

	// openned socket's fd, TO FREE WITH DESTRUCTOR
	std::map<std::string const, int> _opennedSockets;

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
