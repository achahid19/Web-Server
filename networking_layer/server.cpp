#include "server.hpp"
#include "request_parsing.hpp"
#include "utils.hpp"
#include "client.hpp"
#include "network_utils.hpp"

static inline void serverBind( std::string const &host, int port, int serverSocket );

int server::running = true; // flag to control server loop

/**
 * TODO list
 * 
 * Implement a proper error handling mechanism.
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
	this->_events.resize(MAX_EVENTS); // reserve space for events

	this->_epoll_fd = epoll_create1(0);
	this->_opennedFds.insert(std::make_pair("epoll instance", this->_epoll_fd));

	for (
		std::map<std::string, int>::iterator it = this->_listening.begin();
		it != this->_listening.end();
		it++
	) {
		int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		
		INFO_LOGS && std::cout << "Server Socket created: " << serverSocket;
		INFO_LOGS && std::cout << std::endl;
		this->_serverSockets.push_back(serverSocket);
		::serverBind(it->first, it->second, serverSocket);
		listen(serverSocket, SOMAXCONN);
		fcntl(serverSocket, F_SETFL, O_NONBLOCK);
		this->_opennedFds.insert(std::make_pair(
			"listening socket " + ::ft_to_string(serverSocket), serverSocket
		));
		::addEpollEvent(this->_epoll_fd, serverSocket, EPOLLIN);
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

		INFO_LOGS && std::cout << "Ready to handle events: " << std::endl;
		// Events loop
		for (int i = 0; i < event_count; i++) {
			INFO_LOGS && std::cout << "Event on socket: " \
			<< this->_events[i].data.fd << std::endl;

			// --CASE 1--: new client connexion
			if (this->isServerSocket(this->_events[i].data.fd)) {
				addClient(this->_events[i].data.fd);
			}
			else if (this->_events[i].events & (EPOLLIN | EPOLLET)) {
				// --CASE 2--: data from existing client
				INFO_LOGS && std::cout << "Client socket is readable." << std::endl;
				int client_socket = this->_events[i].data.fd;
				client *client = this->_connections[client_socket];
				ssize_t total_bytes = 0;

				parsing_status r = readRequest(client, i, &total_bytes);
				if (r == parsing_status::COMPLETED && total_bytes > 0) {
					struct epoll_event clt_event = this->_events_map[this->_events[i].data.fd];

					::modEpollEvent(
						this->_epoll_fd, client_socket, EPOLLOUT | EPOLLET, &clt_event
					);
					INFO_LOGS && std::cout << "Socket updated to writable." << std::endl;
				}
				else if (total_bytes == 0) {
					// (client disconnected / error client-side)
					INFO_LOGS && std::cout << "closing client socket: " \
					<< client_socket << std::endl;
					epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_socket, NULL);
					close(client_socket);
					this->_opennedFds.erase(
						"client " + ::ft_to_string(client->getClientId())
					);
					this->_connections.erase(client_socket);
					client->decrementNumConx();
				}
				else if (r == parsing_status::IN_PROGRESS) {
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
bool server::addClient( int serverSocket ) {
	int client_socket = accept(serverSocket, NULL, NULL);

	this->_connections.insert(std::make_pair(client_socket, new client(client_socket)));
	int client_id = this->_connections[client_socket]->getClientId();
	this->_opennedFds.insert(std::make_pair(
		"client " + ::ft_to_string(client_id), client_socket
	));

	fcntl(client_socket, F_SETFL, fcntl(client_socket, F_GETFL, 0) | O_NONBLOCK);
	// Watch for input, use Edge-Triggered MODE. (EPOLLET)
	struct epoll_event clt_event = 
		::addEpollEvent(this->_epoll_fd, client_socket, EPOLLIN | EPOLLET);
	this->_events_map.insert(std::make_pair(client_socket, clt_event));
	INFO_LOGS && std::cout << "New connection on socket: " << \
		client_socket << std::endl;
	INFO_LOGS && std::cout << "Number of connections now: " << \
		this->_connections[client_socket]->getNumConx() << std::endl;

	return true;
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
parsing_status	server::readRequest(client *client, int i, ssize_t* total_bytes) {
	char request_buffer[REQ_BUF_SIZE + 1];
	int client_socket = this->_events[i].data.fd;

	ssize_t bytes = 0;

	memset(request_buffer, 0, REQ_BUF_SIZE + 1);

	// Read only buffer size bytes, read again in the next iteration
	// parse the given data, and change state accordingly
	// (states would be checked through the request_parsing class)

	bytes = recv(client_socket, request_buffer, REQ_BUF_SIZE, 0);
	if (bytes < 0) {
		return parsing_status::ERROR; // TO CHANGE LATER FOR APPROPRIATE HANDLING.
	}
	request_buffer[bytes] = '\0';
	REQ_BUFFER_LOGS && std::cout << static_cast<int>(request_buffer[0]) << ", ";
	INFO_LOGS && std::cout << "Received bytes: " << bytes << std::endl;
	*total_bytes += bytes;
	client->reqAppend(request_buffer, bytes);

	client->getRequestParser().parse(client->getRequest());

	if (client->getRequestParser().getStatus() == parsing_status::COMPLETED) {
		START_LINE_LOGS && std::cout << client->getRequestParser().getStartLine() \
			<< std::endl;
		HEADERS_LOGS && std::cout << client->getRequestParser().getHeadersMap().getHeaders() \
			<< std::endl;
		return parsing_status::COMPLETED;
	}
	
	return parsing_status::IN_PROGRESS;
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
 * Return: void.
 */
static inline void serverBind( std::string const &host, int port, int serverSocket ) {
	sockaddr_in	serverAddress;
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(port);
	serverAddress.sin_addr.s_addr = inet_addr(host.c_str());

	int non_blocking_flag = 1;
	setsockopt(
		serverSocket, SOL_SOCKET, SO_REUSEADDR,
		&non_blocking_flag, sizeof(non_blocking_flag)
	);
	bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
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
