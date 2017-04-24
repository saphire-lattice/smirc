#ifndef SMIRC_MUD_CLIENT_H
#define SMIRC_MUD_CLIENT_H
#include <openssl/ossl_typ.h>
#include <pthread.h>

#define MUD_MAX_BUFFER 128

struct minfo {
  int id;
  char* name;

  char* address;
  int port;
  int socket;

  int use_ssl;
  SSL* ssl;

  char* read_buffer;
  char* line_buffer;
  char* user_buffer;
  int line_length;
  int user_length;

  char** irc_buffer;
  int irc_length;

  pthread_t thread;
  struct irc_server* ircserver;

  struct mcp_state* mcp_state;
};

char** get_line_buffer();
void send_line_mud(struct minfo* mud_l, char* line);

int mud_write(struct minfo* mud, char* buffer, int length);
int mud_read(struct minfo* mud);

void* mud_connect(void* arg);


#endif //SMIRC_MUD_CLIENT_H
