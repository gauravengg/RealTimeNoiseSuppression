CC = gcc
CFLAGS = -Wall -pthread -Iringbuffer -Icapture -Iprocessor -Iplayback

SRC = main.c \
      ringbuffer/ringbuffer.c \
      capture/capture.c \
      processor/processor.c \
      playback/playback.c

OBJ = $(SRC:.c=.o)
TARGET = app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
