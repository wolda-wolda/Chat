#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <winsock.h>
#include <io.h>
#include "include/strings.h"
#include <time.h>

#define BUFFER_SIZE 1000
#define SERVER_PORT 42069

static void error_exit(char *errorMessage) {
    fprintf(stderr,"%s: %d\n", errorMessage, WSAGetLastError());
    exit(EXIT_FAILURE);
}
void sen(char buffer[BUFFER_SIZE], SOCKET client){
    int echo_len=0;
    bzero(buffer, sizeof*(buffer));
    gets(buffer);
    fflush(stdin);
    echo_len = strlen(buffer);
    if(strcmp(buffer, "exit")==0){
        closesocket(client);
        exit(0);
    }
    if (send(client, buffer, echo_len, 0) != echo_len){
        error_exit("send() hat eine andere Anzahl von Bytes versendet als erwartet !!!!");
    }else{
        time_t zeit;
        time(&zeit);
        printf("An Server gesendet: %s \t%s", buffer, ctime(&zeit));
    }
}
//TCP Client
int main() {
    SOCKET client;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int echo_len;
    int retcode;

    WORD wVersionRequested = MAKEWORD(1, 1);
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
    }else{
        printf("Verbindung zu %s:%hu erfolgreich hergestellt.\n", inet_ntoa(server_addr.sin_addr),ntohs(server_addr.sin_port));
        do {
            sen(buffer, client);
        } while (1);
    }
}
