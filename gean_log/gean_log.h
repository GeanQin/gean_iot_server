#ifndef __GEAN_LOG_H__
#define __GEAN_LOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "gean_err.h"

#define GEAN_LOG_LEN_MAX 1024

typedef enum {
    GEAN_LOG_NONE = 0,
    GEAN_LOG_TO_FILE = 1,
    GEAN_LOG_TO_STD = 2,
    GEAN_LOG_TO_SERVER = 4, /* 暂时不支持 */
}GEAN_LOG_MODE;

/*
 * 初始化日志模式，支持gean_log_init(GEAN_LOG_TO_FILE|GEAN_LOG_TO_STD)
 * @param[in]
 *      mode    模式
 * @retval
 *      0       成功
 *      <0      失败
 */
GEAN_ERR gean_log_init(GEAN_LOG_MODE mode);
void gean_log_err(const char *format, ...);
void gean_log_info(const char *format, ...);
void gean_log_deinit();

#ifdef __cplusplus
}
#endif

#endif