#include  "server.hpp"

int	main( void ) {
	server	srv;

	srv.server_run();
	if (srv.getStatus() == server_status::ERROR) {
		return 1;
	}
	return 0;
}
