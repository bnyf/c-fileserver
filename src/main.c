#include <event2/event.h>
#include <signal.h>
#include "server.h"

int main(int argc, char* argv[]) {

	int listener;

    listener = server_init();

	struct event_base* base = event_base_new();
    struct event* ev_listen = event_new(base, listener, EV_READ | EV_PERSIST, accept_cb, base);
    event_add(ev_listen, NULL);
    event_base_dispatch(base);

	return 0;
}