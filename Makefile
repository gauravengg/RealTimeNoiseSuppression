CC=gcc
CFLAGS=-Wall -O2

all: bin/realtime_audio

bin/realtime_audio: src/main.c
	mkdir -p bin
	$(CC) $(CFLAGS) src/main.c -o bin/realtime_audio

clean:
	rm -rf bin
