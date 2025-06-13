# Variables
CC = c++

CFLAGS = -std=c++98

SRCS =	main_test.cpp \
		$(addprefix networking_layer/, server.cpp) \
		$(addprefix parsing_layer/, request_parsing.cpp) \
		$(addprefix utils/, utils.cpp)

HEADERS = headers/

# Source files
all: server

server: $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -I$(HEADERS) $(SRCS) -o server

clean:
	rm -f server

fclean: clean

re: fclean all

.PHONY: all clean
