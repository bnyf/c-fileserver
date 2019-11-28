//
// Created by lwx on 11/23/19.
//

#include "FilePresenter.h"
void downLoadFileResponseDemo(){

    char* path = "/home/ubuntu/test.txt";
    char* res = generateFullFileDownLoadResponse(path);

    printf("%s",res); // or send to socket

    free(res); //to avoid memory leak
}


void uploadFileResponseDemo(){

    char* path = "/home/ubuntu/test.txt";
    char content [200]= "--zzzz\r\nContent-Disposition:form-data;name=\"desc\";filename=\"test2\"\r\n\r\n12345678\r\n123456\r\n123\r\n--zzzz--";//http body
    char* boundary = "zzzz";//需要前端解析Content-Type: multipart/form-data; boundary=xxxx得到boundary的值

    char* res = generateFullFileUpLoadResponseWithParseBody(path,content,boundary);

    printf("%s",res); // or send to socket

    free(res);

}

void downLoadFileResponseInChunked(){

    char* path = "/home/ubuntu/test.txt";
    int socket = 0; //the socket num which is returned by accept()

    doChunkedFileDownLoadResponse(path,socket);//因为是分块传输，每次只传输文件的一部分，所以一次调用无法返回所有内容，直接传socket进去，这个函数实现里直接通过套接字send

}


void doChunkedFileUpLoadResponseWithParseBody(){

    char* path = "/home/ubuntu/test.txt";
    int socket = 0; //the socket num which is returned by accept()

    doChunkedFileUpLoadResponseWithParseBody(path,socket); //因为是分块传输，每次只传输文件的一部分，所以一次调用无法返回所有内容，直接传socket进去，这个函数实现里直接通过套接字send,这个解析还有点问题。实现好后，再push一次，但是接口不会变了
}