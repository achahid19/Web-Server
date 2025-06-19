#include "server.hpp"
#include "request_parsing.hpp"
#include "utils.hpp"
#include "client.hpp"
#include "network_utils.hpp"

static inline void serverBind( std::string const &host, int port, int serverSocket );

int server::running = true; // flag to control server event's loop

/**
 * TODO list
 * 
 * methods handling
 * Implement a proper error handling mechanism.
 * c++98 compliance
 * config file
 */

/**
 * server::server - constructor
 * 
 * creating a listening sockets,
 * binding them to an address and port, 
 * and then putting them in a listening state.
 * init the epoll mechanism to handle events.
 */
server::server( void ) {
	this->_listening.insert(std::make_pair("127.0.0.2", 9090));
	this->_listening.insert(std::make_pair("127.0.0.3", 7070));
	this->_listening.insert(std::make_pair("127.0.0.1", 8080));
	this->_events.resize(MAX_EVENTS); // reserve space for events

	this->_epoll_fd = epoll_create1(0);
	if (this->_epoll_fd < 0) {
		throw server_error("Error creating epoll instance");
	}
	this->_opennedFds.insert(std::make_pair("epoll instance", this->_epoll_fd));
	for (
		std::map<std::string, int>::iterator it = this->_listening.begin();
		it != this->_listening.end();
		it++
	) {
		int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket < 0) {
			throw server_error("Error creating listening socket: " + it->first);
		}
		INFO_LOGS && std::cout << "Server Socket created: " << serverSocket;
		INFO_LOGS && std::cout << std::endl;
		this->_serverSockets.push_back(serverSocket);
		::serverBind(it->first, it->second, serverSocket);
		if (listen(serverSocket, SOMAXCONN) < 0) {
			throw server_error("Error listening on socket: " + it->first);
		}
		if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) < 0) {
			throw server_error("Error setting socket to non-blocking: " + it->first);
		}
		this->_opennedFds.insert(std::make_pair(
			"listening socket " + ::ft_to_string(serverSocket), serverSocket
		));
		bool error = false;
		::addEpollEvent(this->_epoll_fd, serverSocket, EPOLLIN, &error);
		error = false;
		if (error) {
			throw server_error("Error adding socket to epoll: " + it->first);
		}
	}
}

/**
 * server::~server - deconstructor
 * 
 * Clean up server resources.
 */
server::~server( void ) {
	for (
		std::map<std::string, int>::iterator it = this->_opennedFds.begin();
		it != this->_opennedFds.end();
		++it
	) {
		close(it->second);
		INFO_LOGS && std::cout << "closed fd: " << it->first << std::endl;
	}
	for (
		std::map<int, client*>::iterator it = this->_connections.begin();
		it != this->_connections.end();
		++it
	) {
		delete it->second;
	}
	this->_opennedFds.clear();
	this->_connections.clear();
	INFO_LOGS && std::cout << "Server destroyed." << std::endl;
};

/**
 * server::server_run - This server method
 * run the server loop, waiting for events
 * on the server sockets, then handling them.
 * 
 * Return: void.
 */
void server::server_run( void ) {
	signal(SIGINT, signalHandler);

	while (running) {
		INFO_LOGS && std::cout << "Incoming connexions / Data..." << std::endl;

		int event_count = epoll_wait(
			this->_epoll_fd, this->_events.data(), MAX_EVENTS, -1
		);
	
		if (event_count < 0) {
			if (errno == EINTR) {
				INFO_LOGS && std::cout << "Epoll wait interrupted by signal." << std::endl;
				continue ;
			}
			std::cerr << "Error in epoll_wait: " << strerror(errno) << std::endl;
			continue ;
		}

		INFO_LOGS && std::cout << "Ready to handle events: " << std::endl;
		// Events loop
		for (int i = 0; i < event_count; i++) {
			INFO_LOGS && std::cout << "Event on socket: " \
			<< this->_events[i].data.fd << std::endl;

			// --CASE 1--: new client connexion
			if (this->isServerSocket(this->_events[i].data.fd)) {
				try {
					addClient(this->_events[i].data.fd);
				}
				catch (const client_connection_error &e) {
					std::cerr << e.what() << std::endl;
					continue;
				}
			}
			else if (this->_events[i].events & (EPOLLIN | EPOLLET)) {
				// --CASE 2--: data from existing client
				INFO_LOGS && std::cout << "Client socket is readable." << std::endl;
				int client_socket = this->_events[i].data.fd;
				client *client = this->_connections[client_socket];
				ssize_t read_bytes = 0;

				parsing_status parser_status = readRequest(client, i, &read_bytes);
				if (parser_status == COMPLETED && read_bytes > 0) {
					struct epoll_event clt_event = this->_events_map[this->_events[i].data.fd];

					::modEpollEvent(
						this->_epoll_fd, client_socket, EPOLLOUT | EPOLLET, &clt_event
					);
					INFO_LOGS && std::cout << "Socket updated to writable." << std::endl;
				}
				else if (read_bytes == 0) {
					// (client disconnected / error client-side)
					INFO_LOGS && std::cout << "closing client socket: " \
						<< client_socket << std::endl;
					if (epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_socket, NULL) < 0) {
						std::cerr << "Error removing client socket from epoll: " \
							<< client_socket << ", " << strerror(errno) << std::endl;
					}
					close(client_socket);
					this->_opennedFds.erase(
						"client " + ::ft_to_string(client->getClientId())
					);
					delete this->_connections[client_socket];
					this->_connections.erase(client_socket);
					client->decrementNumConx();
				}
				else if (parser_status == IN_PROGRESS
						|| parser_status == READING_ERROR)
				{
					struct epoll_event clt_event = this->_events_map[this->_events[i].data.fd];

					::modEpollEvent(
						this->_epoll_fd, client_socket, EPOLLIN | EPOLLET, &clt_event
					);
				}
			}
			else if (this->_events[i].events & (EPOLLOUT | EPOLLET)) {
				// --CASE 3--: client socket is writable, send response
				INFO_LOGS && std::cout << "Client socket is writable." << std::endl;
				int client_socket = this->_events[i].data.fd;
				client *client = this->_connections[client_socket];

				// this is a generic example of processing the request
				{
					/* 
						// we got the request, process it
						INFO_LOGS && std::cout << "------------- DATA ---------------" << std::endl;
						INFO_LOGS && std::cout << client->getRequest();
						INFO_LOGS && std::cout << "------------- DATA ---------------" << std::endl;

						// example: echo server response
						std::string response_body = "Body:" + client->getRequest() + "\n";
						std::string content_length_str = ::ft_to_string(response_body.length());

						std::string http_response = "HTTP/1.1 200 OK\r\n"; // status line
						// HTTP headers
						http_response += "Content-Type: text/plain\r\n"; 
						http_response += "Content-Length: " + content_length_str + "\r\n";
						http_response += "\r\n"; // End of headers
						http_response += response_body; 
						send(client_socket, http_response.c_str(), http_response.length(), 0);
					*/
					
					/* RESPONSE HANDLER GOES HERE */
					
					client->clearReq();
					client->getRequestParser().resetParser();
				}
				// modify the event to listen for input again
				struct epoll_event clt_event = this->_events_map[this->_events[i].data.fd];

				::modEpollEvent(
					this->_epoll_fd, client_socket, EPOLLIN | EPOLLET, &clt_event
				);
			}
		}
	}
}

/**
 * addClient - add a new client to the server.
 * 
 * @serverSocket: listening socket fd.
 * 
 * Return: boolean.
 */
void server::addClient( int serverSocket ) {
	int client_socket = accept(serverSocket, NULL, NULL);

	if (client_socket < 0) {
		throw client_connection_error(
			"Error accepting new client connection on socket: " \
			+ ::ft_to_string(serverSocket)
		);
	}
	fcntl(client_socket, F_SETFL, fcntl(client_socket, F_GETFL, 0) | O_NONBLOCK);
	// Watch for input, use Edge-Triggered MODE. (EPOLLET)
	bool error = false;
	struct epoll_event clt_event = ::addEpollEvent(
		this->_epoll_fd, client_socket, EPOLLIN | EPOLLET, &error
	);
	if (error) {
		close(client_socket);
		throw client_connection_error(
			"Error adding client socket to epoll: " + ::ft_to_string(client_socket)
		);
	}
	this->_connections.insert(std::make_pair(client_socket, new client(client_socket)));
	int client_id = this->_connections[client_socket]->getClientId();
	this->_opennedFds.insert(std::make_pair(
		"client " + ::ft_to_string(client_id), client_socket
	));
	this->_events_map.insert(std::make_pair(client_socket, clt_event));
	INFO_LOGS && std::cout << "New connection on socket: " << \
		client_socket << std::endl;
	INFO_LOGS && std::cout << "Number of connections now: " << \
		this->_connections[client_socket]->getNumConx() << std::endl;
}

/**
 * processRequest - process the request from the client.
 * 
 * @request: reference to the request string to be filled.
 * @i: index of the event in the events vector.
 * @total_bytes: pointer to total bytes received.
 * 
 * Return: void.
 */
parsing_status	server::readRequest(client *client, int i, ssize_t* read_bytes) {
	char request_buffer[REQ_BUF_SIZE + 1];
	int client_socket = this->_events[i].data.fd;
	ssize_t bytes = 0;

	memset(request_buffer, 0, REQ_BUF_SIZE + 1);

	/**
	 * Read only buffer size bytes, read again in the next iteration
	 * parse the given data, and change state accordingly
	 * (states would be checked through the request_parsing class)
	*/

	bytes = recv(client_socket, request_buffer, REQ_BUF_SIZE, 0);
	if (bytes < 0) {
		std::cerr << "Error reading from client socket: " \
			<< client_socket << ", " << strerror(errno) << std::endl;
		return READING_ERROR;
	}
	request_buffer[bytes] = '\0';
	REQ_BUFFER_LOGS && std::cout << static_cast<int>(request_buffer[0]) << ", ";
	INFO_LOGS && std::cout << "Received bytes: " << bytes << std::endl;
	*read_bytes += bytes;
	
	client->reqAppend(request_buffer, bytes);
	client->getRequestParser().parse(client->getRequest());

	if (client->getRequestParser().getStatus() == COMPLETED) {
		REQUEST_LOGS && std::cout << client->getRequestParser().getRequestLine() \
			<< std::endl;
		REQUEST_LOGS && std::cout << client->getRequestParser().getHeadersMap().getHeaders() \
			<< std::endl;
		REQUEST_LOGS && std::cout << client->getRequestParser().getBodyContent() \
			<< std::endl;
		return COMPLETED;
	}
	
	return IN_PROGRESS;
}

/**
 * isServerSocket - check if the given socket is a server socket.
 * 
 * @socket: listening socket fd.
 * 
 * Return: boolean
 */
bool server::isServerSocket( int socket ) {
	for (
		std::vector<int>::iterator it = this->_serverSockets.begin();
		it != this->_serverSockets.end();
		it++
	) {
		if (*it == socket) return true;
	}
	
	return false;
}

/**
 * serverBind - bind the server socket to the specified host and port.
 * 
 * @host: the host address to bind to.
 * @port: the port number to bind to.
 * @serverSocket: listening socket fd.
 * 
 * Return: bool.
 */
static inline void serverBind( std::string const &host, int port, int serverSocket ) {
	sockaddr_in	serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(host.c_str());

	int non_blocking_flag = 1;
	if (
		setsockopt(
			serverSocket, SOL_SOCKET, SO_REUSEADDR,
			&non_blocking_flag, sizeof(non_blocking_flag)
		) < 0
	) {
		throw server::server_error("Error setting socket options for " + host);
	}
	if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
		throw server::server_error(
			"Error binding socket to " + host + ":" + ::ft_to_string(port)
		);
	}
}

/**
 * signalHandler - signal handler for SIGINT
 * 
 * @signal: the signal number (e.g., SIGINT)
 * 
 * Return: void.
 */
void server::signalHandler( int signal ) {
	(void)signal;
	running = false;
}

// exception handling
server::server_error::server_error( const std::string &msg ) : _msg(msg) {};
const char* server::server_error::what() const throw() {
	return _msg.c_str();
}

server::client_connection_error::client_connection_error( const std::string &msg ) : _msg(msg) {};
const char* server::client_connection_error::what() const throw() {
	return _msg.c_str();
}
