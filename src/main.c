#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr()
#include <unistd.h>

#include "server.h"

int main(int argc, char* argv[]) {
	start_serv();	

	return 0;
}

