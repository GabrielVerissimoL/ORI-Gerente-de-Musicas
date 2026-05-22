CC = gcc
CFLAGS = -Wall -g

TARGET = bin/projeto

SRCS = $(shell find . -name "*.c")

INCLUDES = $(shell find . -name "*.h" -exec dirname {} \; | sort -u | sed 's/^/-I/')

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	mkdir -p bin
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	rm -f $(OBJS) $(TARGET)

re: clean $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: clean re run