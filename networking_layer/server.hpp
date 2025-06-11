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
#include <cstring> // strlen() function
#include <csignal> // signal handling


#define MAX_EVENTS 5 // max number of events to handle in epoll in one go

class server {
private:
	int	_serverSocket; // fd socket listenning for new client's connections

	// epoll attributes
	int	_epoll_fd; // interest list, handle to epoll instance
	// what if we have more than max events? # TO CHECK # DONE
	std::vector<struct epoll_event> _events; // events list

	// openned socket's fd, TO FREE WITH DESTRUCTOR
	std::map<std::string const, int> _opennedSockets;

	// signal handler
	static void signalHandler( int signal );
	static int running; // flag to control server loop

	// helper functions to run the server
	bool addClient( void );
	void readRequest(std::string& request, int i, ssize_t* total_bytes);

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
