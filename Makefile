INCLUDE_DIR = -I./gean_common
INCLUDE_DIR += -I./gean_log
INCLUDE_DIR += -I./gean_socket
INCLUDE_DIR += -I./gean_db

SRC_FILE = main.c
SRC_FILE += */*.c

LINK_LIB = -lpthread -lmysqlclient -lcjson

# "需要转义
DEFINE_ARG = -DLOG_FILE=\"/var/log/gean.log\"

TARGET_BIN = gean_iot_server

all:
	gcc ${SRC_FILE} ${INCLUDE_DIR} ${DEFINE_ARG} ${LINK_LIB} -o ${TARGET_BIN}

clean:
	rm -rf ${TARGET_BIN}