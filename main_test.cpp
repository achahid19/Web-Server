#include  "server.hpp"

int	main( int ac, char **av ) {
	if (ac != 2) {
		std::cerr << "Usage: " << av[0] << " <config_file>" << std::endl;
		return 1;
	}
	
	try {
		server srv(av[1]);

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
