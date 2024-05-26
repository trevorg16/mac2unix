CC=cc
CC_OPTS=-std=c99 -Wall -Wextra -O2

TARGET=mac2unix

all: $(TARGET)

$(TARGET):
	$(CC) $(CC_OPTS) mac2unix.c -o $@

.PHONY: clean

clean:
	- rm -f $(TARGET)
