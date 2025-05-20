CC = gcc
CFLAGS = -Wall -g -pthread

# Source files
SRCS = main.c resource_manager.c scheduler.c system_log.c
OBJS = $(SRCS:.c=.o)

# Output executable
TARGET = mahamOS

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f *.o $(TARGET)

