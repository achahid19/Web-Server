#pragma once

/**
 * utils functions for network layer.
 */

#include <sys/epoll.h>
#include <iostream>

struct epoll_event	addEpollEvent(
	int epoll_fd,
	int serverSocket,
	uint32_t events,
	bool *error
);

void	modEpollEvent(
	int epoll_fd, int socket, uint32_t events,
	struct epoll_event *event_obj
);
