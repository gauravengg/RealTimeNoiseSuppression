// whisper_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 50007

int whisper_sock = -1;  // Global socket

int init_whisper_connection() {
    struct sockaddr_in server_addr;

    whisper_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (whisper_sock < 0) {
        perror("Socket creation failed");
        return -1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(whisper_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to Whisper server failed");
        close(whisper_sock);
        whisper_sock = -1;
        return -1;
    }

    return 0;
}

int send_audio_to_whisper(const short *audio, int num_samples) {
    if (whisper_sock == -1) {
        if (init_whisper_connection() < 0)
            return -1;  // Connection failed
    }

    int bytes = num_samples * sizeof(short);
    if (send(whisper_sock, audio, bytes, 0) != bytes) {
        perror("Send failed");
        close(whisper_sock);
        whisper_sock = -1;
        return -1;
    }

    return 0;
}
