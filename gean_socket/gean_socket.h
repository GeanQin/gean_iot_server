#ifndef __GEAN_SOCKET_H__
#define __GEAN_SOCKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gean_err.h"

#define GEAN_SOCKET_PORT 16666
#define GEAN_SOCKET_LISTEN_BACKLOG 10
#define GEAN_SOCKET_RECV_LEN_MAX 1024
#define GEAN_SOCKET_WAIT_CHILD_TIME 5   // 5s

#define GEAN_CLIENT_EXIT_CMD "exit"

GEAN_ERR gean_socket_init();
void gean_socket_accept_start();
void gean_socket_deinit();

#ifdef __cplusplus
}
#endif

#endif