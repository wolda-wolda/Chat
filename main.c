/*C Chat Copyright (C) 2021 Marcel Walder, Manuel Unterhuber
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <winsock.h>
#include <windows.h>
#include "include/strings.h"
#include <time.h>
#include <conio.h>

#define BUFFER_SIZE 10000
#define SERVER_PORT 42069

void error_exit(char *errorMessage);

int echo(SOCKET fd, char echo_buffer[BUFFER_SIZE], time_t zeit);

int sen(SOCKET fd, char buffer[BUFFER_SIZE], time_t zeit);

void sock();

int close = 0;


//TCP Client
int main() {
    sock();
}

void sock() {
    SOCKET sock, fd;
    unsigned int len;
    struct sockaddr_in server_addr, client;
    int retcode;
    char echo_buffer[BUFFER_SIZE] = {0};
    time_t zeit;

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
                do {
                    close = sen(fd, echo_buffer, zeit);
                    if (close != 1) {
                        close = echo(fd, echo_buffer, zeit);
                    }
                } while (close != 1);
                closesocket(fd);
                close = 0;
            }
        }
    }
}

int echo(SOCKET fd, char echo_buffer[BUFFER_SIZE], time_t zeit) {
    int recv_size = -1;
    u_long len;

    bzero(echo_buffer, sizeof *(echo_buffer));
    ioctlsocket(fd, FIONREAD, &len);
    if (len) {
        recv_size = recv(fd, echo_buffer, BUFFER_SIZE, 0);
        echo_buffer[recv_size] = '\0';
        if (strcmp(echo_buffer, "exit") == 0) {
            printf("Client hat die Verbindung getrennt, warte auf neue Verbindung\n");
            close = 1;
            return close;
        } else {
            time(&zeit);
            printf("Nachrichten vom Client : %s \t%s", echo_buffer, ctime(&zeit));
        }
    } else {
        return close;
    }
}

int sen(SOCKET fd, char buffer[BUFFER_SIZE], time_t zeit) {
    int echo_len = 0;
    char firstchar;

    bzero(buffer, sizeof *(buffer));
    fflush(stdin);
    while (1) {
        if (kbhit()) {
            firstchar = getch();
            fgets(buffer, 100, stdin);
            fflush(stdin);
            for (int i = 99; i >= 0; i--) {
                if (buffer[i] != '\000') {
                    buffer[i + 1] = buffer[i];
                }
                if (buffer[i + 1] == '\n') {
                    buffer[i + 1] = '\000';
                }
            }
            buffer[0] = firstchar;
            echo_len = strlen(buffer);
            if (send(fd, buffer, echo_len, 0) != echo_len) {
                printf("Verbindung zum Client verloren,  warte auf neue Verbindung\n");
                close = 1;
                return close;
            } else {
                time(&zeit);
                printf("An Client gesendet: %s \t%s", buffer, ctime(&zeit));
            }
            if (strcmp(buffer, "exit") == 0) {
                close = 1;
                printf("Verbindung wurde getrennt, warte auf neue Verbindung\n");
                return close;
            }
        } else {
            Sleep(100);
            return close;
        }
    }
}

void error_exit(char *errorMessage) {
    fprintf(stderr, "%s: %d\n", errorMessage, WSAGetLastError());
    exit(EXIT_FAILURE);
}
