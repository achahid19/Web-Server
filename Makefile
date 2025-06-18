# Variables
CC = c++

CFLAGS = -std=c++11

SRCS =	main_test.cpp \
		$(addprefix networking_layer/, server.cpp client.cpp network_utils.cpp ) \
		$(addprefix parsing_layer/, request_parsing.cpp request_line.cpp http_headers.cpp ) \
		$(addprefix utils/, utils.cpp )

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
