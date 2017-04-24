#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "server.h"
#include "../itoa.h"
#include "../mud/client.h"

void* server_loop(void* arg) {
    struct irc_server* server = (struct irc_server*) arg;
    int iSetOption = 1;
    struct client** clients = calloc(sizeof(struct client*), MAX_CLIENTS);
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i] = 0;

    //Create socket
    server->socket.socket_description = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
    if (server->socket.socket_description == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    setsockopt(server->socket.socket_description, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption,
               sizeof(iSetOption));

    //Prepare the sockaddr_in structure
    server->socket.addr.sin_family = AF_INET;
    server->socket.addr.sin_addr.s_addr = INADDR_ANY;
    server->socket.addr.sin_port = htons( 8989 );

    //Bind
    if( bind(server->socket.socket_description,(struct sockaddr *)&server->socket.addr , sizeof(server->socket.addr)) < 0) {
        //print the error message
        perror("Bind failed. Error");
        pthread_exit(0);
    }
    puts("Bind done.");

    //Listen
    listen(server->socket.socket_description, 3);

    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    server->socket.desc_size = sizeof(struct sockaddr_in);

    //Init loop vars
    int* client_sock = malloc(sizeof(int));
    struct sockaddr_in client;
    //accept connection from an incoming client
    while (1) {
        printf("S_\n");
        *client_sock = accept(server->socket.socket_description, (struct sockaddr *)&client, (socklen_t*)&server->socket.desc_size);
        if (*client_sock < 0) {
            perror("Accept failed");
            break;
        }
        puts("Connection accepted");
        struct client* cinfo = add_client(server);
        if (cinfo != 0) {
            int sock = *client_sock;
            cinfo->socket = sock;
            printf("Creating client thread..\n");
            int err = pthread_create(&(cinfo->cl_thread), NULL, &client_callback, cinfo);

            if (err != 0)
                printf("Can't create thread: [%s]", strerror(err));
            else
                printf("Thread %lu created successfully\n", cinfo->cl_thread);
            printf("SE\n");
        } else {
            printf("Error: out of client slots!\n");
            close(*client_sock);
        }
    }
    pthread_exit(0);
}

void add_mud(struct minfo* mud) {
    struct irc_mud** next = &(mud->ircserver->mud);
    while (*next != 0) {
        next = &((*next)->next);
    }
    *next = malloc(sizeof(struct irc_mud));
    (*next)->mud = mud;
}

struct minfo* get_mud(struct irc_server* server, char* channel) {
    struct irc_mud** next = &(server->mud);
    while (*next != 0) {
        if (strcmp((*next)->mud->name, channel +1) == 0)
            return (*next)->mud;
        next = &((*next)->next);
    }
    return 0;
}