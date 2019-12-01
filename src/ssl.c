#include "ssl.h"

typedef struct sockaddr SA;

int create_ssl(SSL_CTX **ctx_ptr, SSL **ssl_ptr, int nListenFd)
{


    struct sockaddr_in addr;
    int nAcceptFd;

    SSLeay_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();

    memset(&addr, 0, sizeof(addr));
    socklen_t len  = sizeof(addr);

    nAcceptFd = accept(nListenFd, (struct sockaddr *)&addr, &len);

    *ctx_ptr = SSL_CTX_new (SSLv23_method());
    if( *ctx_ptr == NULL)
    {
        printf("SSL_CTX_new error!\n");
        return -1;
    }

    // 是否要求校验对方证书 此处不验证客户端身份所以为： SSL_VERIFY_NONE
    SSL_CTX_set_verify(*ctx_ptr, SSL_VERIFY_NONE, NULL);

    // 加载CA的证书
    if(!SSL_CTX_load_verify_locations(*ctx_ptr, CA_CERT_FILE, NULL))
    {
        printf("SSL_CTX_load_verify_locations error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 加载自己的证书
    if(SSL_CTX_use_certificate_file(*ctx_ptr, SERVER_CERT_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_certificate_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 加载自己的私钥  私钥的作用是，ssl握手过程中，对客户端发送过来的随机
    //消息进行加密，然后客户端再使用服务器的公钥进行解密，若解密后的原始消息跟
    //客户端发送的消息一直，则认为此服务器是客户端想要链接的服务器
    if(SSL_CTX_use_PrivateKey_file(*ctx_ptr, SERVER_KEY_FILE, SSL_FILETYPE_PEM) <= 0)
    {
        printf("SSL_CTX_use_PrivateKey_file error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    // 判定私钥是否正确
    if(!SSL_CTX_check_private_key(*ctx_ptr))
    {
        printf("SSL_CTX_check_private_key error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    // 将连接付给SSL
    *ssl_ptr = SSL_new (*ctx_ptr);
    if(!(*ssl_ptr))
    {
        printf("SSL_new error!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }
    SSL_set_fd (*ssl_ptr, nAcceptFd);

    int isContinue = 1;
    while(isContinue)
    {
        if(SSL_accept(*ssl_ptr) != 1)
        {
            int icode = -1;
            int iret = SSL_get_error(*ssl_ptr, icode);
            if (iret == SSL_ERROR_WANT_READ)
            {
                isContinue = 1;
            }
            else
            {
                SSL_CTX_free(*ctx_ptr);
                SSL_free(*ssl_ptr);
                *ctx_ptr = NULL;
                *ssl_ptr = NULL;

                break;
            }
        }
        else
            break;
    }

    return nAcceptFd;
}