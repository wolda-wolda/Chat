#include <stdio.h>
#include <winsock.h>
#include <string.h>
#include <stdlib.h>
#include <strings.h>

#define SERVER_PORT 1234

//TCP Client
int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(SERVER_PORT);
}
