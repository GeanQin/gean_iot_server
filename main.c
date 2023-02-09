#include <stdio.h>
#include <signal.h>

#include "gean_log.h"
#include "gean_err.h"
#include "gean_socket.h"
#include "gean_db.h"

static void stop_program(int num)
{
    gean_log_info("[%s] recv signal %d", __func__, num);
    gean_socket_deinit();
    gean_db_deinit();
}

int main(int argc, char **argv)
{
    GEAN_ERR ret = GEAN_NONE_ERR;

    signal(SIGINT, stop_program);

    gean_log_init(GEAN_LOG_TO_STD);
    
    ret = gean_db_init();
    ret |= gean_socket_init();
    if(ret == GEAN_NONE_ERR)
    {
        gean_socket_accept_start();
    }

    return (int)ret;
}