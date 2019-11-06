
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "tools.h"
#include "http.h"
#define __LINEMAXNUM__ 20
/**
 * name: pre_Process
 * desc: 将字符流转化为标准http报文
 * para:
 *     @message     客户端发送的报文字符流
 * return: 请求报文
 * 
 * !!!!!该函数中未实现错误处理部分
*/
request_message pre_Process(char *message)
{
    request_message req;
    /*
     1.根据传送的字符信息流进行拆分字符串
     2.将对应的字符串生成标准请求格式
     3.将生成的请求
    */
    //  分开头部和实体
    int lineNum = 0;
    char *head_body[2] = {0};
    lineNum = split_str(message, __SPLIT_HEADANDBODY__, head_body); // 调用分割函数
    //添加实体
    if(lineNum==1)
        req.body=NULL;
    req.body = head_body[1];
    char *line[__LINEMAXNUM__] = {0};
    lineNum = split_str(head_body[0], CRLF, line); // 调用分割函数

    //生成请求行
    char *req_line[3] = {0};
    int rl_num = split(line[0], " ", req_line);
    req.rl= (request_line *)malloc(sizeof(request_line));// 为请求行分配空间
    req.rl->method = (char *)malloc(sizeof(char) * strlen(req_line[0]));
    strcpy(req.rl->method,req_line[0]);
    req.rl->url = (char *)malloc(sizeof(char) * strlen(req_line[1]));
    strcpy(req.rl->url,req_line[1]);
    req.rl->version = (char *)malloc(sizeof(char) * strlen(req_line[2]));
    strcpy(req.rl->version,req_line[2]);
    
    //  生成请求头部
    req.rh= (request_head *)malloc(sizeof(request_head));// 为请求行分配空间
    for(int i=1;i<lineNum;i++)
    {
        char *rh_head[2]={0};
        int rh_head_num=split(line[i], ":", rh_head);
        /*此处应添加错误处理部分*/
        if(!strcmp(rh_head[0],"Accept-Language"))
        {
            //多行同一类型请求，默认保存最后一次
            req.rh->accept_language=(char *)malloc(sizeof(char) * strlen(rh_head[1]));
            strcpy(req.rh->accept_language,rh_head[1]);
        }
        /*添加其他的首部字段*/
    }
    return req;
}

/**
 * name: pre_Process
 * desc: 生成响应报文
 * para:    @参数待定
 * return: 响应报文
*/
response_message gen_Message()
{
    response_message res;
    return res;
}

/**
 * name: execReq
 * desc: 根据请求报文转入相应处理函数执行
 * para:    @req 标准化的请求报文
 * return: 成功返回0，失败返回-1;
*/
int execReq(request_message * req)
{

    if(!strcmp(req->rl->method,"GET"))
        return do_get(req);
    else if(!strcmp(req->rl->method,"POST"))
        return do_post(req);
    /*此处添加其他方法跳转函数*/
    else
        return -1;
    return 0;
}

/**
 * name: do_get
 * desc: GET方法处理http请求
 * para:    @req 标准化的请求报文
 * return: 成功返回0，失败返回-1;
*/
int do_get(request_message *req)
{
    return 0;
}

/**
 * name: do_post
 * desc: POST方法处理http请求
 * para:    @req 标准化的请求报文
 * return: 成功返回0，失败返回-1;
*/
int do_post(request_message *req)
{
    printf("%s\n",req->body);
	printf("%s\n",req->rl->version);
	printf("%s\n",req->rh->accept_language);
    printf("test_post\n");
    return 0;
}
