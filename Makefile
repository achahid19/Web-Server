# Variables
CC = c++

CFLAGS = -std=c++11

SRCS = main_test.cpp \
	$(addprefix networking_layer/, server.cpp) \
	   $(addprefix parsing_layer/, request_parsing.cpp) \

HEADERS = $(addprefix networking_layer/, server.hpp) \
	   		$(addprefix parsing_layer/, request_parsing.hpp)

# Source files
all: server

server: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o server $(SRCS)

clean:
	rm -f server

fclean: clean

.PHONY: all clean
