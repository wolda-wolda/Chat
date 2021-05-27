#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <winsock.h>
#include <windows.h>
#include <io.h>
#include "include/strings.h"

#define BUFFER_SIZE 1000
#define SERVER_PORT 42069

//TCP Client
int main() {
    unsigned long long client;
    struct sockaddr_in server_addr;
    char buffer[1000];
    int retcode;

    WORD wVersionRequested = MAKEWORD(1, 1);       // Stuff for WSA functions
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);

    client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0)
    {
        printf("Fehler beim Socket erstellen\n");
        exit(-1);
    }
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    retcode = connect(client, (struct sockaddr *)&server_addr,sizeof(server_addr));
    if (retcode < 0)
    {
        printf("Verbindung fehlgeschlagen\n");
        exit(-1);
    }
}
