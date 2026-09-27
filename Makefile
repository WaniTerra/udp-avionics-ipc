CC = gcc
CFLAGS = -Wall -Wextra

all: radar sensor

radar: radar_display.c protocol.h
	$(CC) $(CFLAGS) radar_display.c -o radar -lSDL2

sensor: sensor_node.c protocol.h
	$(CC) $(CFLAGS) sensor_node.c -o sensor

clean:
	rm -rf radar sensor 