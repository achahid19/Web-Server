#include "network_utils.hpp"

/**
 * addEpollEvent - add a new event to the epoll instance
 * 
 * @param epoll_fd: the epoll instance file descriptor.
 * @param socket: the socket fd to monitor.
 * @param events: the events to monitor (EPOLLIN, EPOLLOUT, etc.).
 * 
 * Return: struct epoll_event object containing the event information.
 */
struct epoll_event	addEpollEvent(
	int epoll_fd, int socket, uint32_t events,
	bool *error
) {
	struct epoll_event event_obj;
	event_obj.events = events;
	event_obj.data.fd = socket;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event_obj) < 0) {
		*error = true;
	};
	// for event_obj Kernel making its own copy

	return event_obj;
}

/**
 * modEpollEvent - modify an existing event in the epoll instance
 * 
 * @param epoll_fd: the epoll instance file descriptor.
 * @param socket: the socket fd to modify.
 * @param events: the new events to monitor (EPOLLIN, EPOLLOUT, etc.).
 * @param event_obj: pointer to the epoll_event object to modify.
 * 
 * Return: void.
 */
void	modEpollEvent(
	int epoll_fd, int socket, uint32_t events,
	struct epoll_event *event_obj
) {
	event_obj->events = events;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_MOD, socket, event_obj) < 0) {
		std::cerr << "Error epoll_ctl: unable to modify client socket to reaable." ;
		std::cerr << std::endl;
	}
}
