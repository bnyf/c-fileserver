#ifndef RIO_H
#define RIO_H
/* RIO -- Robust I/O, 减少低字节IO时用户态和内核态的切换 */

#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <stdio.h>
#include <event2/event.h>
#include "ssl.h"

#define RIO_BUFSIZE 4096

typedef struct _Rio{
    int rioFd;      //与缓冲区绑定的文件描述符的编号
    size_t rioCnt;        //缓冲区中还未读取的字节数
    char *rioBufptr;   //当前下一个未读取字符的地址
    char rioBuf[RIO_BUFSIZE];
    struct event* ev;
    SSL_CTX *ctx;
    SSL *ssl;

    ssize_t (*readn)(struct _Rio *rp, void *usrbuf, size_t n);
    ssize_t (*readline)(struct _Rio *rp, void *usrbuf, size_t maxlen);
    ssize_t (*writen)(struct _Rio *rp, void *usrbuf, size_t n);
} Rio;

Rio* newRio(int fd, struct event* ev,  SSL_CTX *ctx, SSL *ssl);
void freeRio(Rio *rp);

#endif

/*
 * example:
 *
 * Rio * rio = newRio(fd，cv);
 * int n = rio -> readn(rio, userbuf, n);
 * freeRio(rio);
 */