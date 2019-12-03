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

int main(int argc, char* argv[]) {

	int listener;

    signal(SIGPIPE, SIG_IGN);

    listener = server_init();

	struct event_base* base = event_base_new();
    struct event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, base);
    event_add(ev_listen, NULL);
    event_base_dispatch(base);

	return 0;
}