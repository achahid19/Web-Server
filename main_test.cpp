#include  "server.hpp"

int	main( void ) {
	
	try {
		server srv;

		srv.server_run();
	}
	catch (const server::server_error &e) {
		std::cerr << "Server error: " << e.what() << std::endl;
		return 1;
	}
	catch (const std::exception &e) {
		std::cerr << "Unexpected error: " << e.what() << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "An unknown error occurred." << std::endl;
		return 1;
	}

	return 0;
}
