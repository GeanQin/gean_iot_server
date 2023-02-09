#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <cjson/cJSON.h>
#include "gean_db.h"

static MYSQL mysql;

GEAN_ERR gean_db_init()
{
    mysql_init(&mysql);

    if (mysql_real_connect(&mysql, "localhost", DB_USER, DB_PASSWD, DB_NAME, 0, NULL, 0) == NULL)
    {
        gean_log_err("[%s] mysql connect error, %s\n", __func__, mysql_error(&mysql));
        return GEAN_DB_CONNECT_ERR;
    }

    mysql_set_character_set(&mysql, "utf8");

    return GEAN_NONE_ERR;
}

GEAN_ERR gean_db_run(char *cmd, char *ret_json, unsigned int ret_len)
{
    int ret = 0;
    MYSQL_RES *result;
    char *ret_str = NULL;

    ret = mysql_real_query(&mysql, cmd, (unsigned int)strlen(cmd));
    if (!ret)
    {
        result = mysql_store_result(&mysql);
        if (result == NULL || ret_json == NULL || ret_len == 0)
        {
            gean_log_info("[%s] run [%s] ok. no ret.", __func__, cmd);
            return GEAN_NONE_ERR;
        }
        gean_log_info("[%s] run [%s] ok.", __func__, cmd);

        int num_fields = mysql_num_fields(result);
        MYSQL_ROW row;
        cJSON* cjson_obj = cJSON_CreateObject();
        cJSON* cjson_results = cJSON_CreateArray();
        int i = 0;

        while ((row = mysql_fetch_row(result)))
        {
            cJSON* cjson_result = cJSON_CreateArray();

            for (i = 0; i < num_fields; i++)
            {
                if (row[i])
                {
                    cJSON_AddItemToArray(cjson_result, cJSON_CreateString(row[i]));
                }
                else
                {
                    cJSON_AddItemToArray(cjson_result, cJSON_CreateString("NULL"));
                }
            }

            cJSON_AddItemToArray(cjson_results, cjson_result);
        }

        cJSON_AddItemToObject(cjson_obj, "result", cjson_results);
        ret_str = cJSON_Print(cjson_obj);
        memcpy(ret_json, ret_str, ret_len);

        cJSON_free(ret_str);
        cJSON_Delete(cjson_obj);
        mysql_free_result(result);
        return GEAN_DB_CMD_RUN_ERR;
    }
    else
    {
        gean_log_err("[%s] run [%s] err", __func__, cmd);
        return GEAN_DB_CMD_RUN_ERR;
    }
}

void gean_db_deinit()
{
    mysql_close(&mysql);
}