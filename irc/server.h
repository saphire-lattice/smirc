#ifndef SMIRC_SERVER_H
#define SMIRC_SERVER_H

struct irc_socket {
  int socket_description;
  int desc_size;
  struct sockaddr_in addr;
};

struct irc_mud {
  struct minfo* mud;
  struct irc_mud* next;
};

struct irc_server {
  char* name;
  struct client** clients;
  struct irc_socket socket;
  struct irc_mud* mud;
};

#include "client.h"

void* server_loop(void*);
void add_mud(struct minfo* mud);
struct minfo* get_mud(struct irc_server* server, char* channel);

#endif //SMIRC_SERVER_H
