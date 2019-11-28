#include "server.h"

int create_server() {
	int socket_desc;
	struct sockaddr_in server;

	// create socket
	if( (socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //IPv4, TCP, IP
		printf("create socket error.\n");
		return 1;
	}

	//init server
	server.sin_family = AF_INET; //IPv4
	server.sin_addr.s_addr =  inet_addr("127.0.0.1");
	server.sin_port = htons(8080);

	//bind
	if( bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0 ){
		printf("bind error.\n");
		return 1;
	}
	printf("bind done.\n");

	//listen
	listen(socket_desc, 3);
	printf("wait for connections...\n");

	return socket_desc;
}