#include "server_socket.hpp"

/**
 * TODO list
 * 
 * handle fails erros properly.
 * free epoll() stuffs.
 */

/**
 * server::server - constructor by parameter
 * creating a socket, binding it to an address and port, 
 * and then putting it in a listening state.
 * @host: ip address where we listening for upcoming connexions
 * @port: openned port for upcoming client's connexions
 * 
 * Return: void.
 */
server::server( std::string const host, int port ) {
	this->_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << "Server Socket created: " << this->_serverSocket << std::endl;

	// specify the server address
	sockaddr_in	serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	/* TODO. check why we used inet_addr for. */
	serverAddress.sin_addr.s_addr = inet_addr(host.c_str());

	// bind the socket to the server address and make it reusable
	int non_blocking_flag = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &non_blocking_flag, sizeof(non_blocking_flag));
	bind(_serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

	// listening on socket
	/**
	 * The SOMAXCONN in the listen() function refers to the backlog parameter,
	 * which specifies the maximum length of the queue for pending connections.
	 */
	listen(_serverSocket, SOMAXCONN);
	// making server socket operations (accept...) non-blocking.
	fcntl(_serverSocket, F_SETFL, O_NONBLOCK);
}

/**
 * server::~server
 */
server::~server( void ) {};

/**
 * server::server_run - connexions / clients sockets management
 * 
 * Return: void.
 */
void server::server_run( void ) {
	while (true) {
		
	}
}
