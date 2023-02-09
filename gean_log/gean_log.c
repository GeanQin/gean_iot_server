#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>

#include "gean_log.h"

static bool std_flag = false;
static char *log_file = "/var/log/gean.log";
static FILE *log_fp = NULL;

static GEAN_ERR gean_log_init_file()
{
#ifdef LOG_FILE
    log_file = LOG_FILE;
#endif
    log_fp = fopen(log_file, "w");
    if (log_fp == NULL)
    {
        gean_log_err("Cannot open log file [%s]", log_file);
        return GEAN_OPEN_FILE_ERR;
    }
    return GEAN_NONE_ERR;
}

GEAN_ERR gean_log_init(GEAN_LOG_MODE mode)
{
    GEAN_ERR ret = GEAN_NONE_ERR;

    switch (mode)
    {
    case GEAN_LOG_TO_STD:
        std_flag = true;
        gean_log_info("[%s] use GEAN_LOG_TO_STD", __func__);
        break;
    case GEAN_LOG_TO_FILE:
        ret = gean_log_init_file();
        gean_log_info("[%s] use GEAN_LOG_TO_FILE", __func__);
        break;
    case GEAN_LOG_TO_STD | GEAN_LOG_TO_FILE:
        std_flag = true;
        ret = gean_log_init_file();
        gean_log_info("[%s] use GEAN_LOG_TO_STD | GEAN_LOG_TO_FILE", __func__);
        break;

    default:
        break;
    }
    return ret;
}

void gean_log_err(const char *format, ...)
{
    char log_str[GEAN_LOG_LEN_MAX] = {0};
    va_list arg;
    va_start(arg, format); /* va_start初始化vaList, 从format开始 */
    time_t time_p;

    time(&time_p);
    snprintf(log_str, GEAN_LOG_LEN_MAX, "[%s", ctime(&time_p));
    log_str[strlen(log_str) - 1] = ']';
    vsnprintf(log_str + strlen(log_str), GEAN_LOG_LEN_MAX, format, arg); /* 配合格式化字符串，输出可变参数 */
    if (std_flag)
    {
        fprintf(stderr, "[ERR]%s\n", log_str);
    }
    if (log_fp != NULL)
    {
        fprintf(log_fp, "[ERR]%s\n", log_str);
        fflush(log_fp);
    }
}

void gean_log_info(const char *format, ...)
{
    char log_str[GEAN_LOG_LEN_MAX] = {0};
    va_list arg;
    va_start(arg, format); /* va_start初始化vaList, 从format开始 */
    time_t time_p;

    time(&time_p);
    snprintf(log_str, GEAN_LOG_LEN_MAX, "[%s", ctime(&time_p));
    log_str[strlen(log_str) - 1] = ']';
    vsnprintf(log_str + strlen(log_str), GEAN_LOG_LEN_MAX, format, arg); /* 配合格式化字符串，输出可变参数 */
    if (std_flag)
    {
        fprintf(stdout, "[INFO]%s\n", log_str);
    }
    if (log_fp != NULL)
    {
        fprintf(log_fp, "[INFO]%s\n", log_str);
        fflush(log_fp);
    }
}

void gean_log_deinit()
{
    if (std_flag)
    {
        std_flag = false;
    }
    if (log_fp != NULL)
    {
        fclose(log_fp);
    }
}