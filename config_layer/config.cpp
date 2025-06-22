#include "config.hpp"
#include <fstream>
#include "utils.hpp"

/**
 * TODO list -
 * - do the server-block needs a mandatori set of directives to run?
 * - need to handle location blocks inside server blocks.
 * - retrieve listenning sockets from _server_config.
 */

 /**
  * Error handling:
  * - seprator between directive and value can be any space or tab. TO HANDLE.
  * - Check if the list of directives is valid.
  */

// constructor
config_file ::config_file( void ) {
	this->_server_block_count = 0;
};

// destructor
config_file ::~config_file( void ) {
	// Clean up server blocks if needed
	_server_blocks.clear();
};

/**
 * loadConfig - Load configuration from a file.
 */
void config_file::loadConfig( const char *config_file ) {
	// Load configuration from file
	_config_file = config_file;
	// Open the file
	std::ifstream file(_config_file);
	if (!file.is_open()) {
		throw std::runtime_error("Could not open config file: " + std::string(_config_file));
	}

	while (true) {
		std::string line;
		std::getline(file, line);
		if (file.eof()) {
			CONFIG_LOGS && std::cout << "End of file reached." << std::endl;
			if (this->_server_block_count == 0) {
				throw std::runtime_error(
					"No server blocks found in config file: " + std::string(_config_file)
				);
			}
			break;
		}
		CONFIG_LOGS && std::cout << "Read line: " << line << std::endl;
		line = ::ft_trim_spaces(line);
		if (line.empty() || line.at(0) == '#') {
			continue; // Skip empty lines and comments
		}
		else if (line.find("server {") != std::string::npos) {
			server_block *block = new server_block();

			while (getline(file, line) && ::ft_trim_spaces(line).find("}") == std::string::npos) {
				line = ::ft_trim_spaces(line);
				CONFIG_LOGS && std::cout << line << std::endl;
				if (line.empty() || line[0] == '#') {
					continue; // Skip empty lines and comments
				}
				else if (line.find("port") != std::string::npos) {
					size_t sep = line.find_first_of(" \t");

					if (sep == std::string::npos) {
						throw std::runtime_error("Invalid port directive in server block: " + line);
					}
					block->setPort(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
				}
				else if (line.find("host") != std::string::npos) {
					size_t sep = line.find_first_of(" \t");

					if (sep == std::string::npos) {
						throw std::runtime_error("Invalid host directive in server block: " + line);
					}
					block->setHost(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
				}
				else if (line.find("server_name") != std::string::npos) {
					size_t sep = line.find_first_of(" \t");

					if (sep == std::string::npos) {
						throw std::runtime_error("Invalid server_name directive in server block: " + line);
					}
					block->setServerName(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
				}
				else if (line.find("root") != std::string::npos) {
					size_t sep = line.find_first_of(" \t");

					if (sep == std::string::npos) {
						throw std::runtime_error("Invalid root directive in server block: " + line);
					}
					block->setRoot(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
				}
				// else if (line.find("client_max_body_size") != std::string::npos) {
				// 	block->setClientMaxBodySize(stoi(line.substr(line.find(' ') + 1)));
				// }
				// else if (line.find("index") != std::string::npos) {
				// 	block->setIndex(line.substr(line.find(' ') + 1));
				// }
				else {
					throw std::runtime_error("Unknown directive in server block: " + line);
				}
			}
			line = ::ft_trim_spaces(line);
			if (line.empty() || line[0] != '}') {
				throw std::runtime_error("Invalid server block in config file: " + std::string(_config_file));
			}
			else {
				this->addServerBlock(block);
				this->_server_block_count++;
			}
		}
		else {
			throw std::runtime_error("Unexpected line in config file: " + line);
		}
	}
	if (CONFIG_LOGS) {
		// Check directives validity.
		for(
			std::vector< const server_block * >::const_iterator it = this->_server_blocks.begin();
			it != this->_server_blocks.end();
			it++
		) {
			std::cout << "Host: " << (*it)->getHost() << std::endl;
			std::cout << "Port: " << (*it)->getPort() << std::endl;
			std::cout << "Server Name: " << (*it)->getServerName() << std::endl;
			std::cout << "Root: " << (*it)->getRoot() << std::endl;
		}
	}
}

void config_file::addServerBlock( const server_block *block ) {
	_server_blocks.push_back(block);
}
