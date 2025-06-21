#include "config.hpp"
#include <fstream>
#include "utils.hpp"

/**
 * TODO list -
 * - do the server-block needs a mandatori set of directives to run?
 * - need to handle location blocks inside server blocks.
 * - retrieve listenning sockets from _server_config.
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
			break; // End of file reached
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
					block->setPort(line.substr(line.find(' ') + 1));
				}
				else if (line.find("host") != std::string::npos) {
					block->setHost(line.substr(line.find(' ') + 1));
				}
				else if (line.find("server_name") != std::string::npos) {
					block->setServerName(line.substr(line.find(' ') + 1));
				}
				else if (line.find("root") != std::string::npos) {
					block->setRoot(line.substr(line.find(' ') + 1));
				}
				// else if (line.find("client_max_body_size") != std::string::npos) {
				// 	block->setClientMaxBodySize(stoi(line.substr(line.find(' ') + 1)));
				// }
				else if (line.find("index") != std::string::npos) {
					block->setIndex(line.substr(line.find(' ') + 1));
				}
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

}

void config_file::addServerBlock( const server_block *block ) {
	_server_blocks.push_back(block);
}
