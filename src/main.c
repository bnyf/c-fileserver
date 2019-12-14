#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr()
#include <unistd.h>
#include <event2/event.h>
#include <signal.h>
#include "server.h"
#include "rio.h"

void handler(int num) {
    //我接受到了SIGCHLD的信号啦
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("The child %d exit with code %d\n", pid, WEXITSTATUS(status));
    }
}

int main(int argc, char* argv[]) {

	int listener;

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, handler);

    listener = server_init();

	struct event_base* base = event_base_new();
    struct event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, base);
    event_add(ev_listen, NULL);
    event_base_dispatch(base);

	return 0;
}