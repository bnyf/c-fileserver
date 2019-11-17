#ifndef RIO_H
#define RIO_H
/* RIO -- Robust I/O, 减少低字节IO时用户态和内核态的切换 */

#include <string.h>
#include <sys/socket.h>

#define RIO_BUFSIZE 4096

typedef struct _Rio{
    int rioFd;      //与缓冲区绑定的文件描述符的编号
    size_t rioCnt;        //缓冲区中还未读取的字节数
    char *rioBufptr;   //当前下一个未读取字符的地址
    char rioBuf[RIO_BUFSIZE];

    ssize_t (*readn)(Rio *rp, void *usrbuf, size_t n);
    ssize_t (*readline)(Rio *rp, void *usrbuf, size_t maxlen);
    ssize_t (*writen)(int fd, void *usrbuf, size_t n);
}Rio; 

Rio* newRio(int fd);
static ssize_t Rio_read(Rio *rp, char *usrbuf, size_t n);


#endif

/*
    Rio * rio = newRio(fd);
    rio -> readn(rio, userbuf, n);
*/