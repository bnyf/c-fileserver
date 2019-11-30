#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h> //inet_addr()
#include <unistd.h>
#include <rio.h>
#include <event2/event.h>

#include "get_ip.h"
#include "http.h"

int server_init();
void accept_cb(int fd, short events, void* arg);
void socket_read_cb(int fd, short events, void* arg);

#endif