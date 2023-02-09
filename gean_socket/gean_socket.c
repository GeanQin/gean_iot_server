#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <pthread.h>

#include "gean_socket.h"
#include "gean_log.h"
#include "gean_db.h"

static bool accept_flag = false;
static int server_sockfd = -1;

GEAN_ERR gean_socket_init()
{
    int ret = 0;
    struct sockaddr_in server_addr;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd == -1)
    {
        gean_log_err("[%s] socket create err", __func__);
        return GEAN_CREATE_SOCKET_ERR;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(GEAN_SOCKET_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero), sizeof(server_addr.sin_zero));

    ret = bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret != 0)
    {
        gean_log_err("[%s] socket bind err", __func__);
        return GEAN_BIND_SOCKET_ERR;
    }

    ret = listen(server_sockfd, GEAN_SOCKET_LISTEN_BACKLOG);
    if (ret != 0)
    {
        gean_log_err("[%s] socket listen err\n", __func__);
        return GEAN_LISTEN_SOCKET_ERR;
    }

    return GEAN_NONE_ERR;
}

static void *wait_thread(void *arg)
{
    int child_status = 0;
    while(accept_flag)
    {   /* 每隔一段时间清理一下僵尸进程 */
        waitpid(-1, &child_status, WNOHANG);
        sleep(GEAN_SOCKET_WAIT_CHILD_TIME);
    }
}

static bool mac_is_ok(char *mac)
{
    int i = 0;

    if(strlen(mac) < 16)
    {
        return false;
    }

    for(i = 0; i < 16; i++)
    {
        if ((mac[i] >= '0' && mac[i] <= '9') || (mac[i] >= 'A' && mac[i] <= 'F'))
        {
            continue;
        }
        else
        {
            return false;
        }
    }

    return true;
}

static GEAN_ERR mac2db(char *mac)
{
    char cmd[256] = {0};

    snprintf(cmd, sizeof(cmd), "delete from client_pid where did='%s'", mac);
    gean_db_run(cmd, NULL, 0);

    memset(cmd, 0 , sizeof(cmd));
    snprintf(cmd, sizeof(cmd), "insert into client_pid(did, pid) values('%s', %d)", mac, getpid());
    return gean_db_run(cmd, NULL, 0);
}

static void sigcb(int signo)
{
    switch (signo)
    {
    case SIGUSR1:
        printf("[%d]Get a signal -- SIGUSR1\n", getpid());
        break;
    case SIGUSR2:
        printf("[%d]Get a signal -- SIGUSR2\n", getpid());
        break;
    }
    return;
}

void gean_socket_accept_start()
{
    GEAN_ERR ret = GEAN_NONE_ERR;
    int client_sockfd = -1;
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    pid_t child_pid;
    char rec_str[GEAN_SOCKET_RECV_LEN_MAX] = {0};
    int rec_len = 0;
    pthread_t wait_pid = 0;

    accept_flag = true;

    if(pthread_create(&wait_pid, NULL, wait_thread, NULL) != 0)
    {
        gean_log_err("[%s] cannot create thread to wait child process", __func__);
    }

    while (accept_flag)
    {
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_sockfd == -1)
        {
            gean_log_err("[%s] socket accept err", __func__);
            break;
        }
        if ((child_pid = fork()) == 0)
        {
            gean_log_info("[%s]create pid(%d)", __func__, getpid());

            rec_len = recv(client_sockfd, rec_str, GEAN_SOCKET_RECV_LEN_MAX, 0);
            if (rec_len > 16)
            {
                rec_str[16] = 0;
                if (mac_is_ok(rec_str))
                {
                    gean_log_info("[%s]recv mac ok [%s]", __func__, rec_str);
                    write(client_sockfd, rec_str, rec_len);

                    ret = mac2db(rec_str);
                    if (ret != GEAN_NONE_ERR)
                    {
                        gean_log_err("[%s]write mac to db err [%s]", __func__, rec_str);
                        accept_flag = false;
                    }
                    else
                    {
                        signal(SIGUSR1, sigcb);
                        signal(SIGUSR2, sigcb);
                    }
                }
                else
                {
                    gean_log_info("[%s]recv mac err [%s]", __func__, rec_str);
                    accept_flag = false;
                }
            }
            else
            {
                accept_flag = false;
            }

            while (accept_flag)
            {
                memset(rec_str, 0, GEAN_SOCKET_RECV_LEN_MAX);
                rec_len = recv(client_sockfd, rec_str, GEAN_SOCKET_RECV_LEN_MAX, 0);
                if(rec_len > 0)
                {
                    rec_str[strlen(rec_str) - 1] = 0;
                    gean_log_info("[%s]recv str is [%s], len=%d", __func__, rec_str, strlen(rec_str));
                    if (0 == strncmp(GEAN_CLIENT_EXIT_CMD, rec_str, strlen(GEAN_CLIENT_EXIT_CMD) + 1))
                    {
                        break;
                    }
                    write(client_sockfd, rec_str, rec_len);
                    
                }
            }
            close(client_sockfd);
            gean_log_info("[%s]pid(%d) exit", __func__, getpid());
            _exit(0);
        }
        else
        {
            close(client_sockfd);
        }
    }
}

void gean_socket_deinit()
{
    accept_flag = false;
    close(server_sockfd);
}