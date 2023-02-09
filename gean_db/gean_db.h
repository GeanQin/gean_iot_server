#ifndef __GEAN_LOG_H__
#define __GEAN_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gean_err.h"

#define DB_NAME "gean_iot_server"
#define DB_USER "root"
#define DB_PASSWD "root.806"

GEAN_ERR gean_db_init();
GEAN_ERR gean_db_run(char *cmd, char *ret_json, unsigned int ret_len);
void gean_db_deinit();

#ifdef __cplusplus
}
#endif

#endif