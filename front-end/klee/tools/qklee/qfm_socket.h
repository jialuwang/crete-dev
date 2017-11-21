#ifndef _QFM_SOCKET_H_
#define _QFM_SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

#define QFM_SOCKET_ADDRESS "/tmp/qfm_socket"

int create_qfm_socket(int isServer);

int wait_for_qfm_client(int s);

void close_qfm_socket(int s);

FILE *open_qfm_socket(int s);

void sendMessage(int s, char * buf, int len);

int recvMessage(FILE * fp, char * buf);

#endif
