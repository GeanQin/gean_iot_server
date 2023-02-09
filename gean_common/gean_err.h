#ifndef __GEAN_ERR_H__
#define __GEAN_ERR_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    GEAN_NONE_ERR = 0,
    GEAN_OPEN_FILE_ERR = -9999,
    GEAN_WRITE_FILE_ERR,
    GEAN_CREATE_SOCKET_ERR,
    GEAN_BIND_SOCKET_ERR,
    GEAN_LISTEN_SOCKET_ERR,
    GEAN_DB_CONNECT_ERR,
    GEAN_DB_CMD_RUN_ERR,

}GEAN_ERR;

#ifdef __cplusplus
}
#endif

#endif