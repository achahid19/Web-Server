#pragma once

/**
 * common utils for the project header
 */

#include <iostream>
#include <sstream>
#include <sys/epoll.h>

// logging flags
enum LogFlags {
	REQ_BUFFER_LOGS = false,
	DEBUG_LOGS = false,
	INFO_LOGS = false,
	REQUEST_LOGS = false,
	CONFIG_LOGS = true
};

// turn an int to a string in a c++98 compliant way
std::string	ft_to_string(int value);
std::string	ft_trim_spaces( const std::string &str );
