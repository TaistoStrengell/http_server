CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread -D_GNU_SOURCE
LDFLAGS = -pthread


TARGET = server
SRCS = main.c threadpool.c http_handler.c
OBJS = $(SRCS:.c=.o)
HEADERS = threadpool.h http_handler.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

valgrind: $(TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TARGET)

.PHONY: all clean valgrind