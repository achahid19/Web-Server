#include "server_socket.hpp"

int	main( void ) {
	server	srv("127.0.0.1", 8080);

	srv.server_run();
	return 0;
}
