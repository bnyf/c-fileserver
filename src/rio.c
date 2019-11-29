#include "rio.h"
/**
 * @brief Rio_read  RIO--Robust I/O底层读取函数。当缓冲区数据充足时，此函数直接拷贝缓
 *                  冲区的数据给上层读取函数；当缓冲区不足时，该函数通过系统调用
 *                  从文件中读取最大数量的字节到缓冲区，再拷贝缓冲区数据给上层函数
 *
 * @param rp        rio，里面包含了文件描述符和其对应的缓冲区数据
 * @param usrbuf    读取的目的地址
 * @param n         读取的字节数量
 *
 * @returns         返回真正读取到的字节数(<=n)。 socket关闭回0, 读取失败返回-1。
 */
static ssize_t Rio_read(Rio *rp, char *usrbuf, size_t n) {
    ssize_t readCnt;
    size_t copyCnt;

    while(rp->rioCnt <= 0) {
//        printf("begin read: %d\n",rp ->rioCnt);
        readCnt = read(rp->rioFd, rp->rioBuf, sizeof(rp->rioBuf));
//        printf("end read: %d\n",readCnt);
        if(readCnt < 0) {
            if(errno != EINTR)
                return -1;
        }
        else if(readCnt == 0)   //socket 关闭
            return 0;
        else {
            rp->rioBufptr = rp->rioBuf;
            rp->rioCnt = readCnt;
        }
    }

    copyCnt = n;
    if(rp->rioCnt < n)     
        copyCnt = rp->rioCnt;
    memcpy(usrbuf, rp->rioBufptr, copyCnt);
    rp->rioBufptr += copyCnt;      //读取后需要更新指针
    rp->rioCnt -= copyCnt;         //未读取字节也会减少

    return copyCnt;
}

/**
 * @brief Rio_readn     供用户使用的读取函数。从缓冲区中读取最大maxlen字节数据,阻塞 io 直到读够maxlen。
 *
 * @param rp            rio_t，文件描述符与其对应的缓冲区
 * @param usrbuf        void *, 目的地址
 * @param n             size_t, 用户想要读取的字节数量
 *
 * @returns             真正读取到的字节数。成功回n, socket关闭回或读取失败返回<n。
 */
ssize_t Rio_readn(Rio *rp, void *usrbuf, size_t n) {
    size_t leftCnt = n;
    ssize_t readCnt;
    char *bufp = (char *)usrbuf;

    while(leftCnt > 0) {
        readCnt = Rio_read(rp, bufp, leftCnt);
        if(readCnt < 0) {
            return -1;
        }
        else if(readCnt == 0){
            break;
        }
        else{
//            printf("readn: %d\n",readCnt);
            leftCnt -= readCnt;
            bufp += readCnt;
        }
    }

    return n-leftCnt;
}

/**
 * @brief Rio_readline 读取一行的数据，遇到'\n\r'结尾代表一行。阻塞 io，直到读到'\n\r'。
 *
 * @param rp            rio包
 * @param usrbuf        用户地址，即目的地址
 * @param maxlen        size_t, 一行最大的长度。若一行数据超过最大长度，则以'\0'截断
 *
 * @returns             真正读取到的字符数量
 */
ssize_t Rio_readline(Rio *rp, void *usrbuf, size_t maxlen) {
    size_t totCnt = 0;
    ssize_t readCnt;
    char c, *bufp = (char *)usrbuf;

    for(int i=0; i<maxlen; i++) {   //n代表已接收字符的数量
        if((readCnt=Rio_read(rp, &c, 1)) == 1) {
            if(c == '\r')
                continue;
            if(c == '\n')
                break;
            ++totCnt;
            *bufp = c;
            ++bufp;
        }
        else if(readCnt == 0)        //socket 关闭
            break;
        else                    
            return -1;
    }
    *bufp = 0;

    return totCnt;
}

/**
 * @brief Rio_writen     供用户使用的写函数。
 *
 * @param fd            文件描述符
 * @param usrbuf        void *, 源地址
 * @param n             size_t, 用户想要写的字节数量
 *
 * @returns             真正写的字节数。
 */
ssize_t Rio_writen(Rio *rp, void *usrbuf, size_t n) {
    size_t leftCnt = n;
    ssize_t writeCnt;
    char *bufp = (char *)usrbuf;

    while(leftCnt > 0) {
        if((writeCnt = write(rp->rioFd, bufp, leftCnt)) <= 0) {
            if(errno == EINTR)
                writeCnt = 0;
            else
                return -1;
        }
        bufp += writeCnt;
        leftCnt -= writeCnt;
    }

    return n;
}


/**
 * @brief newRio  rio 结构体初始化,并绑定文件描述符、缓冲区和函数指针
 *
 * @param rp        io结构体
 * @param fd        文件描述符
 */
Rio* newRio(int fd) {
    Rio* rio = malloc(sizeof(Rio));
    rio->rioFd = fd;
    rio->rioCnt = 0;
    rio->rioBufptr = rio->rioBuf;
    rio->readn = Rio_readn;
    rio->readline = Rio_readline;
    rio->writen = Rio_writen;

    return rio;
}

void freeRio(Rio *rp) {
    free(rp);
    close(rp->rioFd);
}
