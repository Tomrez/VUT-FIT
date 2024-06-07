CC = gcc
CFLAGS = -std=gnu17 -Wall -Wextra -Werror -pedantic

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

TARGET = ipk-sniffer

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) -lpcap

clean:
	$(RM) $(TARGET) $(OBJS)
	$(RM) $(TARGET)