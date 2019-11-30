#ifndef __HTTP_H__
#define __HTTP_H__

#include "tools.h"
#include "url_parser.h"
#include "rio.h"

#define LF '\n'
#define CR '\r'
#define CRLF "\r\n\0"
#define __SPLIT_HEADANDBODY__ "\r\n\r\n\0"
#define __LINEMAXNUM__ 20
#define __ERROR__ 0
#define __OK__ 1
#define  __CLOSED__ 2
//定义状态码
#define __NORMAL__ 200
#define __REQUEST_ERROR__ 400
#define __INTERNAL_SERVER_ERROR__ 500
#define __MAXLENGTH__ 4096
/**
 * http 协议的基本数据结构
*/

//http协议使用的方法（该数据结构未使用）
enum METHOD {
    GET,
    POST,
    HEAD,
    PUT,
    DELETE,
    OPTIONS,
    TRACE,
    CONNECT
};
//http请求行
typedef struct {
    char *method;
    char *url;
    char *version;
} request_line;

/**
 * http请求头部
 * 下列为http/1.1规定的所有头部，不知道哪些是会用到的，全部列在下面。
 * 需要使用相应的头部字段，将注释去掉即可
 */
typedef struct {
    char *host;                 //请求资源所在服务器
    char *connection;        //逐跳首部、连接的管理
    char *content_length;       //标识post报文主体长度
    char *content_type;       //标识post报文主体类型
    //char *accept;               //用户可处理的媒体类型
    //char *accept_language;      //优先的自然语言
    // char *accept_encoding;      //优先的内容编码
    // char *accept_charset;       //优先的字符集
    // char *authorization;        //web认证信息
    // char *expect;               //期待服务器特定行为
    // char *from;                 //用户的电子邮箱地址
    // char *User_agent;           //客户端标识信息（系统版本，浏览器，内核等）
    // char *referer;              //来源，即通过什么页面或文件触发的请求，如果是浏览器地址栏回车则没有该值
    // char *range;                //指定返回信息范围（断点持续子类使用）
    // char *if_match;             //比较实体标记
    // char *if_range;             //资源未更新时发送实体Byte的范围请求
    // char *if_modifiled_since;   //缓存相关，本地文件的标识有效期
    // char *if_unmodifiled_since; //缓存相关，本地文件的标识有效期
    // char *if_none_match;        //缓存相关，本地文件的特征码，对应返回信息中的ETag
    // char *te;                   //传输编码的优先级
    // char *max_forwards;         //最大传输逐条数
    // char *proxy_authorization;  //代理服务器要求客户端的认证信息
    // char *Cookie;               //特殊的信息存储位置，用于自动交互，无需代码干涉

    //通用首部字段
    // char *cache_control;     //控制缓存行为

    // char *data;              //创建报文的日期时间
    // char *pragma;            //报文指令
    // char *trailer;           //报文末端的首部一览
    // char *transfer_encoding; //指定报文主体的传输编码
    // char *upgrade;           //升级为其他协议
    // char *via;               //代理服务器的相关信息
    // char *warning;           //错误通知
} request_head;

//http请求报文
typedef struct {
    request_line *rl;
    request_head *rh;
    char *body;
} request_message;

//http状态行
typedef struct {
    char *version;
    int status_code;
    char *reason_phrase;
} status_line;

/**
 * http响应头部
 * 下列为http/1.1规定的所有头部，不知道哪些是会用到的，全部列在下面。
 * 需要使用相应的头部字段，将注释去掉即可
 */
typedef struct {
    //通用首部字段
    // char *cache_control;     //控制缓存行为
    // char *connection;        //逐跳首部、连接的管理
    // char *data;              //创建报文的日期时间
    // char *pragma;            //报文指令
    // char *trailer;           //报文末端的首部一览
    // char *transfer_encoding; //指定报文主体的传输编码
    // char *upgrade;           //升级为其他协议
    // char *via;               //代理服务器的相关信息
    // char *warning;           //错误通知

    //响应首部字段
    // char *accept_ranges;      //是否接受字节范围请求
    // char *age;                //推算资源创建经过时间
    // char *etag;               //资源匹配信息
    // char *location;           //令客户端重定向至指定URI
    // char *proxy_authenticate; //代理服务器对客户端对认证信息
    // char *retry_after;        //对再次发起对请求对时机要求
    // char *server;             //http服务器安装信息
    // char *vary;               //代理服务器缓存管理信息
    // char *www_authenticate;   //服务器对客户端的认证信息
    // char *albw;               //资源可支持的http方法
    // char *content_Encoding;   //实体主体适用的编码方式
    // char *content_language;   //实体主体的自然语言
    // char *content_length;     //实体主体的（字节）大小
    // char *content_location;   //替代对应资源的URI
    // char *content_MD5;        //实体主体的报文摘要
    // char *content_range;      //实体的位置范围
    // char *content_type;       //实体主体的媒体类型
    // char *expires;            //实体主体过期的日期时间
    char *last_modified;      //资源最后修改日期的时间

} response_headers;

//http响应报文
typedef struct {
    status_line *sl;
    response_headers *rh;
    char *body;
} response_message;

//-----------------------------------------------------------------------------------
/*从tcp读取http流*/
uint32_t read_http(Rio *rio, int *statue_code);

/*将字符流转化为标准http报文*/
request_message pre_Process(char *message, int *error);

/*生成响应报文*/
uint32_t send_Message(Rio *rio, int *statue_code, const char *res,int res_len);

/*根据请求报文转入相应处理函数执行*/
uint32_t execReq(request_message *req, Rio *rio, int *statue_code);

/*执行GET方法处理请求*/
uint32_t do_get(request_message *req, Rio *rio, int *statue_code);

/*执行POST方法处理请求*/
uint32_t do_post(request_message *req, Rio *rio, int *statue_code);

#endif