#include "server_socket.hpp"

int server::running = true; // flag to control server loop

/**
 * TODO list
 * 
 * implement epoll mechanism
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
	this->_events.resize(MAX_EVENTS); // reserve space for events
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

	// set up epoll for monitoring events on the server socket
	this->_epoll_fd = epoll_create1(0);

	struct epoll_event cnx_event;
	cnx_event.events = EPOLLIN; // interested in read events / upcoming (connexions / data)
	cnx_event.data.fd = this->_serverSocket; // the server socket fd

	epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, this->_serverSocket, &cnx_event);
	
	this->_opennedSockets.insert({"epoll instance", this->_epoll_fd});
	this->_opennedSockets.insert({"listening socket", this->_serverSocket});
}

/**
 * server::~server
 */
server::~server( void ) {
	for (auto& socket : this->_opennedSockets) {
		close(socket.second);
		std::cout << "closed fd: " << socket.first << std::endl;
	}
	this->_opennedSockets.clear();
	std::cout << "Server destroyed." << std::endl;
};


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

/**
 * server::server_run - connexions / clients sockets management
 * 
 * Return: void.
 */
void server::server_run( void ) {
	// Register the signal handler for SIGINT.
	signal(SIGINT, signalHandler);

	while (running) {
		std::cout << "Waiting for clients incoming connexions / Data..." << std::endl;

		// this would block until an event occurs
		int event_count = epoll_wait(this->_epoll_fd, this->_events.data(), MAX_EVENTS, -1);

		std::cout << "Ready to handle events: " << std::endl;
		// a loop to handle each ready event
		for (int i = 0; i < event_count; i++) {
			std::cout << "Event on socket: " << this->_events[i].data.fd << std::endl;

			// --CASE 1--: new client connexion
			if (this->_events[i].data.fd == this->_serverSocket) {
				int client_socket = accept(this->_serverSocket, nullptr, nullptr);
				this->_opennedSockets.insert({"client " + std::to_string(client_socket), client_socket});

				// non-blocking socket
				fcntl(client_socket, F_SETFL, fcntl(client_socket, F_GETFL, 0) | O_NONBLOCK);
				struct epoll_event clt_event;

				clt_event.data.fd = client_socket;
				clt_event.events = EPOLLIN | EPOLLET; // Watch for input, use Edge-Triggered
				epoll_ctl(this->_epoll_fd, EPOLL_CTL_ADD, client_socket, &clt_event);
				std::cout << "New connection on socket: " << client_socket << std::endl;
			}
			// else is a client socket ready for reading data.
			else {
				// --CASE 2--: data from existing client
				int client_socket = this->_events[i].data.fd;
				char request_buffer[1024]; // request_buffer to hold incoming data
				std::string request;

				ssize_t bytes = 0;
				ssize_t total_bytes = 0;
				while (true) {
					request_buffer[0] = '\0'; // clear the buffer
					bytes = recv(client_socket, request_buffer, sizeof(request_buffer) - 1, 0);
					if (request_buffer[0] == '\0') break; // means bytes == 0 or -1 (error).
					request_buffer[bytes] = '\0'; // null-terminate the string
					total_bytes += bytes;
					std::cout << "Received bytes: " << bytes << std::endl;
					request.append(request_buffer, bytes);
				}
				if (total_bytes > 0) {
					// we got the request, process it
					std::cout << "------------- DATA ---------------" << std::endl;
					std::cout << request;
					std::cout << "------------- DATA ---------------" << std::endl;

					// example: echo server response
					std::string response_body = "Body:" + request + "\n";
					std::string content_length_str = std::to_string(response_body.length());

					std::string http_response = "HTTP/1.1 200 OK\r\n"; // status line
					// HTTP headers
					http_response += "Content-Type: text/plain\r\n"; 
					http_response += "Content-Length: " + content_length_str + "\r\n";
					http_response += "\r\n"; // End of headers
					http_response += response_body;

					send(client_socket, http_response.c_str(), http_response.length(), 0);
				}
				else if (total_bytes == 0) {
					// no data received, close the client socket
					std::cout << "closing client socket: " << client_socket << std::endl;
					epoll_ctl(this->_epoll_fd, EPOLL_CTL_DEL, client_socket, nullptr);
					close(client_socket);
					this->_opennedSockets.erase("client " + std::to_string(client_socket));
				}
			}
		}
		/* for (auto &event : this->_events) {
			std::cout << "evend fd: " << event.data.fd << std::endl;
		} */
	}
}
