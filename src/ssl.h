#ifndef SSL_H
#define SSL_H

#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#define SERVER_PORT 8080
#define CA_CERT_FILE "server/ca.crt"
#define SERVER_CERT_FILE "server/server.crt"
#define SERVER_KEY_FILE "server/server.key"

int create_ssl(SSL_CTX **ctx_ptr, SSL **ssl_ptr, int nListenFd);

#endif