CC = gcc
CFLAGS = -Wall -Wextra -pthread -Iheaders
SRCS = code/main.c code/http_handler.c code/parser.c code/threadpool.c
OBJS = $(SRCS:.c=.o)
TARGET = server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f code/*.o $(TARGET)