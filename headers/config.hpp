#pragma once

#include <iostream>
#include <server_block.hpp>
#include <vector>
#include <fstream>

class config_file {
private:
	std::vector< const server_block * >	_server_blocks; // NEED TO FREE WITH DESTRUCTOR.
	const char 					*_config_file_path;
	int						_server_block_count;
	std::ifstream					_file;

	// no copy, no instantiation without config file.
	config_file( const config_file &copy );
	config_file& operator=( const config_file &copy );

	// helper methods
	void	_retrieveServerBlocks( void );
	void	_addPortDirective( const std::string &line, server_block *block );
	void	_addHostDirective( const std::string &line, server_block *block );
	void	_addServerNameDirective( const std::string &line, server_block *block );
	void	_addRootDirective( const std::string &line, server_block *block );
	void	_directivesCheckList( void );

public:
	config_file( void );
	~config_file( void );

	// methods
	void	loadConfig( const char *config_file );
	void	addServerBlock( const server_block *block );
	void	parseServerBlock( const std::string &block_str, server_block *block );

	// exception handling
	class config_error : public std::exception {
	private:
		const std::string _msg;
	public:
		config_error( const std::string &msg );
		~config_error( void ) throw();
		virtual const char* what() const throw();
	};
};
