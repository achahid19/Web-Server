#include "network_utils.hpp"

struct epoll_event	addEpollEvent(
	int epoll_fd, int socket, uint32_t events
) {
	struct epoll_event event_obj;
	event_obj.events = events; // interested in read events / upcoming (connexions / data)
	event_obj.data.fd = socket; // the server socket fd

	epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket, &event_obj);
	// for cnx_event Kerne making its own copy

	return event_obj;
}

void	modEpollEvent(
	int epoll_fd, int socket, uint32_t events,
	struct epoll_event *event_obj
) {
	event_obj->events = events;
	epoll_ctl(epoll_fd, EPOLL_CTL_MOD, socket, event_obj);
}
