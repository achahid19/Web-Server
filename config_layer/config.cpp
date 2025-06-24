#include "config.hpp"
#include "utils.hpp"

/**
 * TODO list -
 * - do the server-block needs a mandatori set of directives to run? LOGIC DONE
 * - need to handle location blocks inside server blocks. LATER
 * - retrieve listenning sockets from _server_config. DONE.
 * - Set server block as default. IN PROGRESS
 */

 /**
  * Error handling:
  * - seprator between directive and value can be any space or tab. TO HANDLE. DONE
  * - Check if the list of directives is valid. DONE
  */

// constructor
config_file ::config_file( void ) {
	this->_server_block_count = 0;
};

// destructor
config_file ::~config_file( void ) {
	this->_server_blocks.clear();
	_file.close();
	CONFIG_LOGS && std::cout << "Config file destroyed." << std::endl;
	this->_config_file_path = NULL;
	this->_server_block_count = 0;
	this->_file.clear();
};

/**
 * loadConfig - Load configuration from a file.
 */
void config_file::loadConfig( const char *config_file ) {
	_config_file_path = config_file;
	this->_file.open(_config_file_path);

	if (!this->_file.is_open()) {
		throw config_error("Could not open config file: " + std::string(_config_file_path));
	}
	while (true) {
		std::string line;
		std::getline(this->_file, line);
		if (this->_file.eof()) {
			CONFIG_LOGS && std::cout << "End of file reached." << std::endl;
			if (this->_server_block_count == 0) {
				throw config_error(
					"No server blocks found in config file: " + std::string(_config_file_path)
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
			this->_retrieveServerBlocks();
		}
		else {
			throw config_error("Unexpected line in config file: " + line);
		}
	}
	this->_directivesCheckList();
}

void config_file::addServerBlock( const server_block &block ) {
	_server_blocks.push_back(block);
}

// helper methods

void	config_file::_retrieveServerBlocks( void ) {
	server_block	block;
	std::string		line;

	while (
		getline(this->_file, line) 
		&& ::ft_trim_spaces(line).find("}") == std::string::npos
	) {
		line = ::ft_trim_spaces(line);
		CONFIG_LOGS && std::cout << line << std::endl;
		if (line.empty() || line[0] == '#') {
			continue; // Skip empty lines and comments
		}
		else if (line.find("port") != std::string::npos) {
			this->_addPortDirective(line, &block);
		}
		else if (line.find("host") != std::string::npos) {
			this->_addHostDirective(line, &block);
		}
		else if (line.find("server_name") != std::string::npos) {
			this->_addServerNameDirective(line, &block);
		}
		else if (line.find("root") != std::string::npos) {
			this->_addRootDirective(line, &block);
		}
		else if (line.find("index") != std::string::npos) {
		 	this->_addIndexDirective(line, &block);
		}
		else if (line.find("client_max_body_size") != std::string::npos) {
			this->_addClientMaxBodySizeDirective(line, &block);
		}
		else {
			throw config_error("Unknown directive in server block: " + line);
		}
	}
	line = ::ft_trim_spaces(line);
	if (line.empty() || line[0] != '}') {
		throw config_error("Invalid server block in config file: " + std::string(_config_file_path));
	}
	else {
		this->addServerBlock(block);
		this->_server_block_count++;
	}
}

void	config_file::_directivesCheckList( void ) {
	this->_uniqueHostPort.clear();
	for (
		std::vector< server_block >::iterator it = this->_server_blocks.begin();
		it != this->_server_blocks.end();
		it++
	) {
		it->getHost();
		it->getPort();
		if (this->_uniqueHostPort[it->get_host_safe()] != it->get_port_safe()) {
			this->_uniqueHostPort[it->get_host_safe()] = it->get_port_safe();
			// set this server block as default for unique host:port
			it->setAsDefault();
		}
		it->getRoot();
		it->getIndex();
	}
}

void	config_file::_addPortDirective( const std::string &line, server_block *block ) {
	this->_checkSeparator(line);
	block->setPort(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
}

void	config_file::_addHostDirective( const std::string &line, server_block *block ) {
	this->_checkSeparator(line);
	block->setHost(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
}

void	config_file::_addServerNameDirective( const std::string &line, server_block *block ) {
	this->_checkSeparator(line);
	block->setServerName(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
}

void	config_file::_addRootDirective( const std::string &line, server_block *block ) {
	this->_checkSeparator(line);
	block->setRoot(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
}

void	config_file::_addIndexDirective( const std::string &line, server_block *block ) {
	this->_checkSeparator(line);
	block->setIndex(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
}

void	config_file::_addClientMaxBodySizeDirective( const std::string &line, server_block *block ) {
	this->_checkSeparator(line);
	block->setClientMaxBodySize(::ft_trim_spaces(line.substr(line.find_first_of(" \t") + 1)));
}

void	config_file::_checkSeparator( const std::string &line ) {
	if (line.find_first_of(" \t") == std::string::npos) {
		throw config_error("Invalid directive in server block: " + line);
	}
}

// exception handling
config_file::config_error::config_error( const std::string &msg ) : _msg(msg) {};

const char* config_file::config_error::what() const throw() {
	return _msg.c_str();
}
