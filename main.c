#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <winsock.h>
#include <windows.h>
#include <io.h>
#include "include/strings.h"
#include <fcntl.h>
#include <time.h>

#define BUFFER_SIZE 10000
#define SERVER_PORT 42069

static void error_exit(char *errorMessage) {
    fprintf(stderr, "%s: %d\n", errorMessage, WSAGetLastError());
    exit(EXIT_FAILURE);
}

static void echo(SOCKET);

static void echo(SOCKET client_socket) {

    char echo_buffer[BUFFER_SIZE];
    int recv_size = -1;
    u_long len;
    time_t zeit;
    do {
        ioctlsocket(client_socket, FIONREAD, &len);
        if (len) {
            recv_size = recv(client_socket, echo_buffer, BUFFER_SIZE, 0);
            if (recv_size == 0) {
                printf("Client hat die Verbindung getrennt\n");
            } else {
                echo_buffer[recv_size] = '\0';
                time(&zeit);
                printf("Nachrichten vom Client : %s \t%s", echo_buffer, ctime(&zeit));
            }
        }
    } while (recv_size!=0);
}

//TCP Client
    int main() {
        SOCKET sock, fd;
        unsigned int len;
        struct sockaddr_in server_addr, client;
        int retcode;

        WORD wVersionRequested = MAKEWORD(1, 1);
        WSADATA wsaData;
        WSAStartup(wVersionRequested, &wsaData);

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            printf("Fehler beim Socket erstellen\n");
            exit(-1);
        }
        bzero((char *) &server_addr, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = INADDR_ANY;
        server_addr.sin_port = htons(SERVER_PORT);
        retcode = bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr));


        if (retcode < 0) {
            perror("Socket konnte nicht gebunden werden!\n");
            exit(-1);
        } else {
            printf("Socket erfolgreich erstellt und gebunden!\n");
            listen(sock, 2);

            for (;;) {
                len = sizeof(client);
                fd = accept(sock, (struct sockaddr *) &client, &len);
                if (fd < 0) {
                    perror("Fehler bei accept");
                } else {
                    printf("Client verbunden: %s\n", inet_ntoa(client.sin_addr));
                    echo(fd);
                    closesocket(fd);
                }
            }
        }
    }
