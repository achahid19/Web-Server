#pragma once

#include <iostream>
#include <server_block.hpp>
#include <vector>

class config_file {
private:
	std::vector< const server_block * >	_server_blocks; // NEED TO FREE WITH DESTRUCTOR.
	const char 					*_config_file;
	int						_server_block_count;

	// no copy, no instantiation without config file.
	config_file( const config_file &copy );
	config_file& operator=( const config_file &copy );

public:
	config_file( void );
	~config_file( void );

	// methods
	void	loadConfig( const char *config_file );
	void	addServerBlock( const server_block *block );
	void	parseServerBlock( const std::string &block_str, server_block *block );
};
