#include "server.h"
#include "response.h"

int server_init() {
    int socket_desc;
    struct sockaddr_in server;

    // create socket
    if ((socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //IPv4, TCP, IP
        printf("create socket error.\n");
        return 1;
    }

    //允许多次绑定同一个地址。要用在socket和bind之间
    evutil_make_listen_socket_reuseable(socket_desc);

    //init server
    server.sin_family = AF_INET; //IPv4
    char local_ip[INET_ADDRSTRLEN] = {"127.0.0.1"};
    get_ip(local_ip);
    printf("local ip: %s\n", local_ip);
    server.sin_addr.s_addr = inet_addr(local_ip);
    server.sin_port = htons(8080);

    //bind
    if (bind(socket_desc, (struct sockaddr *) &server, sizeof(server)) < 0) {
        printf("bind error.\n");
        return 1;
    }
    printf("bind done.\n");

    //listen
    if (listen(socket_desc, 3) == -1) {
        printf("listen error");
        return 1;
    }
    printf("wait for connections...\n");

    //跨平台统一接口，将套接字设置为非阻塞状态
    evutil_make_socket_nonblocking(socket_desc);


    return socket_desc;
}

void accept_cb(int fd, short events, void *arg) {
    evutil_socket_t sockfd;

    struct sockaddr_in client;
    socklen_t len = sizeof(client);

    sockfd = accept(fd, (struct sockaddr *) &client, &len);
    evutil_make_socket_nonblocking(sockfd);

    printf("accept a client %d\n", sockfd);
    pid_t pid;
    pid=fork();
    if(pid==0){
        return ;
    }
    struct event_base *base = (struct event_base *) arg;
    //动态创建一个event结构体，并将其作为回调参数传递给
    struct event *ev = event_new(NULL, -1, 0, NULL, NULL);
    Rio *rio = newRio(sockfd, ev);
    event_assign(ev, base, sockfd, EV_READ | EV_PERSIST | EV_ET, socket_read_cb, (void *) rio);

    event_add(ev, NULL);
}

void exec_http(void *arg){
    Rio *rio = (Rio *) arg;

    uint32_t statue_code;
    //根据错误状态码回送响应报文
    uint32_t ret_status=read_http(rio, &statue_code);

    if (ret_status!= 1) {
        freeRio(rio);
    }
}

void socket_read_cb(int fd, short events, void *arg) {

    pthread_t tid;
    tid = pthread_create(&tid, NULL, exec_http, arg);
}