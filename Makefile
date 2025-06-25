CC = gcc
CFLAGS = -Wall -g -pthread -Iringbuffer -Icapture -Iprocessor -Iplayback -Iwhisper -lasound
LIBS = -lasound   # <-- ADD THIS

SRC = main.c \
      ringbuffer/ringbuffer.c \
      capture/capture.c \
      processor/processor.c \
      playback/playback.c \
      whisper/whisper_client.c  

OBJ = $(SRC:.c=.o)
TARGET = app

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)    # <-- Link with ALSA here
      


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
