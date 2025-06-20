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
#include "client.hpp"
#include "request_parsing.hpp"
#include <exception> // for exception handling
#include <cerrno>

#define MAX_EVENTS 5 // max number of events to handle in epoll in one go
#define REQ_BUF_SIZE 4096 // request buffer size, to hold incoming data

class client;

class server {
private:
	std::vector< int >			_serverSockets; // fd socket listenning for new client's connections
	std::map< int, client* >	_connections;

	// for testing, need to read config for this purpose
	std::map< std::string, int > _listening;

	// epoll attributes
	int									_epoll_fd; // interest list, handle for epoll instance
	std::vector<struct epoll_event>		_events; // events list // redundance
	std::map<int, struct epoll_event>	_events_map; // map to store events with their fd

	// openned socket's fd, TO FREE WITH DESTRUCTOR
	std::map<std::string const, int> _opennedFds;

	// signal handler
	static void signalHandler( int signal );
	static int running; // flag to control server loop

	// helper functions to run the server
	void 					addClient( int );
	parsing_status			readRequest(client *client, int i, ssize_t* total_bytes);
	bool					isServerSocket( int socket );

	// dont instantiate without param
	// no copy
	server( server& copy );
	server&	operator&( server& copy );

public:
	server( void );
	~server( void );

	void	server_run( void );

	// exception handling
	class server_error : public std::exception {
		private:
			const std::string _msg;
		public:
			server_error( const std::string &msg );
			~server_error( void ) throw() {};
			virtual const char* what() const throw();
	};
	
	class client_connection_error : public std::exception {
		private:
			const std::string _msg;
		public:
			client_connection_error( const std::string &msg );
			~client_connection_error( void ) throw() {};
			virtual const char* what() const throw();
	};
};
