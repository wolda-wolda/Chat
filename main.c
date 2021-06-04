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
#include "include/strings.h"
#include <time.h>
#include <conio.h>

#define BUFFER_SIZE 1000
#define SERVER_PORT 42069

int echo(SOCKET client,char buffer[BUFFER_SIZE], time_t zeit);

void error_exit(char *errorMessage);

int sen(char buffer[BUFFER_SIZE], SOCKET client, time_t zeit);

void sock();

int close=0;

int main() {
    sock();
}

void sock() {
    SOCKET client;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE] = {0};
    int retcode;
    time_t zeit;

    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        printf("Fehler beim Socket erstellen\n");
        exit(-1);
    }
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    retcode = connect(client, (struct sockaddr *) &server_addr, sizeof(server_addr));
    if (retcode < 0) {
        error_exit("Verbindung fehlgeschlagen\n");
        exit(-1);
    } else {
        printf("Verbindung zu %s:%hu erfolgreich hergestellt.\n", inet_ntoa(server_addr.sin_addr),
               ntohs(server_addr.sin_port));
        do{
            close=sen(buffer, client, zeit);
            if (close!=1){
                close=echo(client, buffer, zeit);
            }
        }while (close!=1);
    }
}

int echo(SOCKET client,char echo_buffer[BUFFER_SIZE], time_t zeit) {
    int recv_size = -1;
    u_long len;

    bzero(echo_buffer, sizeof *(echo_buffer));
    ioctlsocket(client, FIONREAD, &len);
    if (len) {
        recv_size = recv(client, echo_buffer, BUFFER_SIZE, 0);
        echo_buffer[recv_size] = '\0';
        if (strcmp(echo_buffer, "exit") == 0) {
            close=1;
            printf("Server hat die Verbindung getrennt\n");
            return close;
        } else {
            time(&zeit);
            printf("Nachrichten vom Server : %s \t%s", echo_buffer, ctime(&zeit));
        }
    } else {
        return close;
    }
}

int sen(char buffer[BUFFER_SIZE], SOCKET client, time_t zeit) {
    char firstchar;
    int echo_len = 0;

    bzero(buffer, sizeof *(buffer));
    while (1) {
        if (kbhit()) {
            firstchar = getch();
            fgets(buffer, 100, stdin);
            fflush(stdin);
            for (int i = 99; i >= 0; i--) {
                if (buffer[i] != '\000') {
                    buffer[i + 1] = buffer[i];
                    if (buffer[i + 1] == '\n') {
                        buffer[i + 1] = '\000';
                    }
                }
            }
            buffer[0] = firstchar;
            echo_len = strlen(buffer);
            if (send(client, buffer, echo_len, 0) != echo_len) {
                printf("Verbindung zum Server verloren\n");
                close = 1;
                return close;
            } else {
                time(&zeit);
                printf("An Server gesendet: %s \t%s", buffer, ctime(&zeit));
            }
            if (strcmp(buffer, "exit") == 0) {
                close=1;
                closesocket(client);
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
