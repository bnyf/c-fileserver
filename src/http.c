
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "tools.h"
#include "http.h"


/**
 * name: pre_Process
 * desc: 将字符流转化为标准http报文
 * para:
 *     @message     客户端发送的报文字符流
 *     @error       状态码
 * return: 请求报文
*/
request_message pre_Process(char *message,int * error)
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
    /*当请求项错误的时候，直接退出，并置状态码*/
    if(lineNum==0)
    {
        *error=__REQUEST_ERROR__;
        return req;
    }
    //添加实体
    if(lineNum==1)
        req.body=NULL;
    req.body = head_body[1];
    char *line[__LINEMAXNUM__] = {0};
    lineNum = split_str(head_body[0], CRLF, line); // 调用分割函数

    //生成请求行
    char *req_line[3] = {0};
    int rl_num = split(line[0], " ", req_line);
    /*当请求项错误的时候，直接退出，并置状态码*/
    if(rl_num!=3)
    {
        *error=__REQUEST_ERROR__;
        return req;
    }
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
        /*当请求项错误的时候，直接退出，并置状态码*/
        if(rh_head_num!=2)
        {
            *error=__REQUEST_ERROR__;
            return req;
        }
        if(!strcmp(rh_head[0],"Accept-Language"))
        {
            //多行同一类型请求，默认保存最后一次
            req.rh->accept_language=(char *)malloc(sizeof(char) * strlen(rh_head[1]));
            strcpy(req.rh->accept_language,rh_head[1]);
        }
        /*添加其他的首部字段*/
    }
    /*正常执行,置状态码为200*/
    *error=__NORMAL__;
    return req;
}

/**
 * name: gen_Message
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
    //解析URL
    url_data_t *url_info = NULL;
    url_info = url_parse(req->rl->url);
    //url_data_inspect(url_info);
    char *filepath=url_info->pathname;
    printf("filepath:%s\n",filepath);
    /*将文件解析地址传入处理函数,需要配合响应函数写*/
    
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
    //如果是POST方法，读取Content-Length字段

    //如果读取不到，发送请求错误报文；如果读取正常，发送200的响应报文

    //正常响应设置管道，进行数据流传送
    return 0;
}
