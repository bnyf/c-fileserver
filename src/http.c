
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
request_message pre_Process(char *message, uint32_t *error) {
    request_message req;
    /*
     1.根据传送的字符信息流进行拆分字符串
     2.将对应的字符串生成标准请求格式
     3.将生成的请求
    */
    //  分开头部和实体
    uint32_t lineNum = 0;
    char *head_body[2] = {0};
    lineNum = split_str(message, __SPLIT_HEADANDBODY__, head_body); // 调用分割函数
    /*当请求项错误的时候，直接退出，并置状态码*/
    if (lineNum == 0) {
        *error = __REQUEST_ERROR__;
        return req;
    }
    //添加实体
    if (lineNum == 1)
        req.body = NULL;
    req.body = head_body[1];
    char *line[__LINEMAXNUM__] = {0};
    lineNum = split_str(head_body[0], CRLF, line); // 调用分割函数

    //生成请求行
    char *req_line[3] = {0};
    uint32_t rl_num = split(line[0], " ", req_line);
    /*当请求项错误的时候，直接退出，并置状态码*/
    if (rl_num != 3) {
        *error = __REQUEST_ERROR__;
        return req;
    }
    req.rl = (request_line *) malloc(sizeof(request_line));// 为请求行分配空间
    req.rl->method = (char *) malloc(sizeof(char) * strlen(req_line[0]));
    strcpy(req.rl->method, req_line[0]);
    req.rl->url = (char *) malloc(sizeof(char) * strlen(req_line[1]));
    strcpy(req.rl->url, req_line[1]);
    req.rl->version = (char *) malloc(sizeof(char) * strlen(req_line[2]));
    strcpy(req.rl->version, req_line[2]);

    //  生成请求头部
    req.rh = (request_head *) malloc(sizeof(request_head));// 为请求行分配空间
    for (int i = 1; i < lineNum; i++) {
        char *rh_head[2] = {0};
        int rh_head_num = split(line[i], ":", rh_head);
        /*当请求项错误的时候，直接退出，并置状态码*/
        if (rh_head_num != 2) {
            *error = __REQUEST_ERROR__;
            return req;
        }
        /*添加其他的首部字段*/
    }
    /*正常执行,置状态码为200*/
    *error = __NORMAL__;
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
uint32_t read_http(Rio *rio, uint32_t *statue_code) {
    request_message req;
    req.body = NULL;

    char data[__MAXLENGTH__];//数据接收

    //初始化socket接口
    int n = 0;
    //第一次读取请求行
    n = rio->readline(rio, data, __MAXLENGTH__);
    printf("readline:%d %s\n", n, data);

    //处理请求行
    char *req_line[3] = {0};

    int rl_num = split(data, " ", req_line);

    if (rl_num != 3) {
        *statue_code = __REQUEST_ERROR__;
        //生成状态码，并返回
        return __ERROR__;
    }
    req.rl = (request_line *) malloc(sizeof(request_line));// 为请求行分配空间
    req.rl->method = (char *) malloc(sizeof(char) * (strlen(req_line[0]) + 1));
    strcpy(req.rl->method, req_line[0]);
    req.rl->url = (char *) malloc(sizeof(char) * (strlen(req_line[1]) + 1));
    strcpy(req.rl->url, req_line[1]);
    req.rl->version = (char *) malloc(sizeof(char) * (strlen(req_line[2]) + 1));
    strcpy(req.rl->version, req_line[2]);

    int res = execReq(&req, rio, statue_code);

    if (!*statue_code)
        *statue_code = 200;

    if (!strcmp(req.rh->connection, "closed")) {
        if (res == __OK__)
            res = __CLOSED__;
    }

    //释放请求行
    free(req.rl->method);
    req.rl->method = NULL;
    free(req.rl->url);
    req.rl->url = NULL;
    free(req.rl->version);
    req.rl->version = NULL;
    free(req.rl);
    req.rl = NULL;
    //释放请求头部
    free(req.rh->connection);
    req.rh->connection = NULL;
    free(req.rh->host);
    req.rh->host = NULL;
    free(req.rh->content_type);
    req.rh->content_type = NULL;

    free(req.body);
    req.body = NULL;
    //默认返回ok
    return res;
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
uint32_t send_Message(Rio *rio, uint32_t *statue_code, const char *res, uint32_t res_len) {
    rio->writen(rio, res, res_len);
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
uint32_t execReq(request_message *req, Rio *rio, uint32_t *statue_code) {
    //读取头部字段,保留keep——alive字段
    char head[__MAXLENGTH__];
    //为请求头部分配空间
    req->rh = (request_head *) malloc(sizeof(request_head));
    //初始化头部指针
    req->rh->host = NULL;
    req->rh->content_length = 0;
    req->rh->connection = NULL;
    req->rh->content_type = NULL;

    while (true) {
        uint32_t count = 0;
        count = rio->readline(rio, head, __MAXLENGTH__);
        printf("request:%d %s\n", count, head);

        if (count == 0) { break; }
        //解析头部
        char *rh_head[2] = {0};
        uint32_t rh_head_num = split_str(head, ": ", rh_head);
        /*当请求项错误的时候，直接退出，并置状态码*/
        if (rh_head_num != 2) {
            //函数返回
            *statue_code = __REQUEST_ERROR__;
            return __ERROR__;
        }
        //复制
        if (!strcmp(rh_head[0], "Content-Length")) {
            //多行同一类型请求，默认保存最后一次
            req->rh->content_length = strtoInt(rh_head[1]);
        }
        if (!strcmp(rh_head[0], "Connection")) {
            //多行同一类型请求，默认保存最后一次
            req->rh->connection = (char *) malloc(sizeof(char) * (strlen(rh_head[1]) + 1));
            strcpy(req->rh->connection, rh_head[1]);
        }
        if (!strcmp(rh_head[0], "Host")) {
            //多行同一类型请求，默认保存最后一次
            req->rh->host = (char *) malloc(sizeof(char) * (strlen(rh_head[1]) + 1));
            strcpy(req->rh->host, rh_head[1]);
        }
        if (!strcmp(rh_head[0], "Content-Type")) {
            //多行同一类型请求，默认保存最后一次
            req->rh->content_type = (char *) malloc(sizeof(char) * (strlen(rh_head[1]) + 1));
            strcpy(req->rh->content_type, rh_head[1]);
        }
    }
    //跳转相应函数处理
    if (!strcmp(req->rl->method, "GET"))
        return do_get(req, rio, statue_code);
    else if (!strcmp(req->rl->method, "POST"))
        return do_post(req, rio, statue_code);
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
uint32_t do_get(request_message *req, Rio *rio, uint32_t *statue_code) {

    //解析URL
    url_data_t *url_info = NULL;
    url_info = url_parse(req->rl->url);
    //    url_data_inspect(url_info);
    char *filepath = url_info->pathname;
    printf("filepath:%s\n", filepath);
    /*将文件解析地址传入处理函数,需要配合响应函数写*/
    uint32_t res_length = 0;
    char *res_seq = generateFullFileDownLoadResponse(filepath, req->rh, &res_length);
    printf("res_seq:%s\n", res_seq);
    uint32_t res = send_Message(rio, statue_code, res_seq, res_length);
    free(res_seq);
    //判断返回值
    if (!res) {
        *statue_code = __INTERNAL_SERVER_ERROR__;
        return __ERROR__;
    }
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
uint32_t do_post(request_message *req, Rio *rio, uint32_t *statue_code) {

    //如果读取不到，发送请求错误报文；如果读取正常，发送200的响应报文
    if (req->rh->content_length == 0) {
        *statue_code = __REQUEST_ERROR__;
        printf("POST Request not found Content_Length!!");
        return __ERROR__;
    }
    //读取body
    req->body = (char *) malloc(sizeof(char) * req->rh->content_length);
    rio->readn(rio, req->body, req->rh->content_length);

    //解析URL
    url_data_t *url_info = NULL;
    url_info = url_parse(req->rl->url);
    //url_data_inspect(url_info);
    char *filepath = url_info->pathname;
    printf("filepath:%s\n", filepath);

    char *res_seq = NULL;
    uint32_t res_length = 0;
    if (strstr(req->rh->content_type, "multipart/form-data") != NULL) {
        //解析content
        char *boundry = strstr(req->rh->content_type, "=") + 1;
        printf("Boundry:%s\n", boundry);
        res_seq = generateFullFileUpLoadResponseWithParseBody(filepath, req->body, boundry, req->rh, &res_length);
    }
    else
    {
        *statue_code = __INTERNAL_SERVER_ERROR__;
        return __ERROR__;
    }

    printf("req_seq:%s\n", res_seq);
    uint32_t res = send_Message(rio, statue_code, res_seq, res_length);
    free(res_seq);

    //判断返回值
    if (!res) {
        *statue_code = __INTERNAL_SERVER_ERROR__;
        return __ERROR__;
    }
    return __OK__;

}
