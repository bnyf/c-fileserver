
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "tools.h"
#include "http.h"
#include "rio.h"
#include "FilePresenter.h"

/**
 * name: pre_Process
 * desc: 将字符流转化为标准http报文(该函数作废)
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
 * name: read_http
 * desc: 从tcp的缓冲区读取数据
 * para:
 *     @fd     socket描述符
 *     @statue_code       状态码
 * return: 请求报文
*/
uint32_t read_http(int fd,int *statue_code)
{
    request_message req;

    char * data;//数据接收
    //初始化socket接口
    Rio *rio = newRio(fd); 
    //第一次读取请求行
    rio->readline(rio,data, __MAXLENGTH__);

    //处理请求行
    char *req_line[3] = {0};
    int rl_num = split(data, " ", req_line);
    if(rl_num!=3)
    {
        *statue_code=__REQUEST_ERROR__;
        //生成状态码，并返回
        return __ERROR__;
    }
    req.rl= (request_line *)malloc(sizeof(request_line));// 为请求行分配空间
    req.rl->method = (char *)malloc(sizeof(char) * strlen(req_line[0]));
    strcpy(req.rl->method,req_line[0]);
    req.rl->url = (char *)malloc(sizeof(char) * strlen(req_line[1]));
    strcpy(req.rl->url,req_line[1]);
    req.rl->version = (char *)malloc(sizeof(char) * strlen(req_line[2]));
    strcpy(req.rl->version,req_line[2]);

    //默认返回ok
    return  execReq(&req,fd,statue_code);
}


/**
 * name: send_Message
 * desc: 发送响应报文
 * para:    
 *         @fd socket描述符
 *         @statue_code 状态码
 *         @res 响应报文字符流
 * return: 响应报文
*/
uint32_t send_Message(int fd,int statue_code,char * res)
{
    //创建socket接口
    Rio *rio = newRio(fd); 
    uint32_t res_len=strlen(res);
    rio->writen(rio,res,res_len);
    return __OK__;
}

/**
 * name: execReq
 * desc: 根据请求报文转入相应处理函数执行
 * para:    
 *         @req 标准化的请求报文
 *         @fd socket描述符
 *         @statue_code 状态码
 * return: 成功返回0，失败返回-1;
*/
uint32_t execReq(request_message * req,int fd,int *statue_code)
{
    if(!strcmp(req->rl->method,"GET"))
        return do_get(req,fd,statue_code);
    else if(!strcmp(req->rl->method,"POST"))
        return do_post(req,fd,statue_code);
    /*此处添加其他方法跳转函数*/
    else
        return __ERROR__;
}

/**
 * name: do_get
 * desc: GET方法处理http请求
 * para:    
 *         @req 标准化的请求报文
 *         @fd socket描述符
 *         @statue_code 状态码
 * return: 成功返回0，失败返回-1;
*/
uint32_t do_get(request_message *req,int fd,int * statue_code)
{
    //读取头部字段并丢弃
    char * head;
    Rio *rio = newRio(fd); 
    while(true)
    {
        rio->readline(rio,head, __MAXLENGTH__);
        if(strlen(head)==0){
            break;
        }

    }
    //解析URL
    url_data_t *url_info = NULL;
    url_info = url_parse(req->rl->url);
    //url_data_inspect(url_info);
    char *filepath=url_info->pathname;
    //printf("filepath:%s\n",filepath);
    /*将文件解析地址传入处理函数,需要配合响应函数写*/
    char *res_seq=generateFullFileDownLoadResponse(filepath);
    if(!send_Message(fd,*statue_code,res_seq))
    {
        *statue_code=__INTERNAL_SERVER_ERROR__;
        return __ERROR__;
    }
    free(filepath);
    free(res_seq);
    return __OK__;
}

/**
 * name: do_post
 * desc: POST方法处理http请求
 * para:    
 *         @req 标准化的请求报文
 *         @fd socket描述符
 *         @statue_code 状态码
 * return: 成功返回0，失败返回-1;
*/
uint32_t do_post(request_message *req,int fd,int *statue_code)
{
    //如果是POST方法，读取Content-Length字段
    //读取头部字段并丢弃
    char * head;
    Rio *rio = newRio(fd); 
    req->rh= (request_head *)malloc(sizeof(request_head));// 为请求头部分配空间
    while(true)
    {
        rio->readline(rio,head, __MAXLENGTH__);
        if(strlen(head)==0){
            break;
        }
        //寻找content-length
        char *rh_head[2]={0};
        int rh_head_num=split(head, ":", rh_head);
        /*当请求项错误的时候，直接退出，并置状态码*/
        if(rh_head_num!=2)
        {
            *statue_code=__REQUEST_ERROR__;
            return __ERROR__;
        }
        //复制
        if(!strcmp(rh_head[0],"Content-Length"))
        {
            //多行同一类型请求，默认保存最后一次
            req->rh->content_length=(char *)malloc(sizeof(char) * strlen(rh_head[1]));
            strcpy(req->rh->content_length,rh_head[1]);
        }
    }
    //如果读取不到，发送请求错误报文；如果读取正常，发送200的响应报文

    if(req->rh->content_length==NULL)
    {
        *statue_code=__REQUEST_ERROR__;
        printf("POST Request not found Content_Length!!");
        return __ERROR__;
    }
    
    ///////上载文件
    

    return __OK__;
}
