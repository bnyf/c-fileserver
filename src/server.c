#include "server.h"

void pre_Process(int socket_desc) {
	printf("do_http\n");
	close(socket_desc);
}


int start_serv() {
	int socket_desc, new_socket = 0;
	struct sockaddr_in server, client;
	char *message, reply[2000];
	socklen_t new_socket_len;

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

	//accept and send data
	while( (new_socket = accept(socket_desc, (struct sockaddr*)&client, (socklen_t*)(&new_socket_len) )) ) {
		pre_Process(new_socket);
	}
	if(new_socket == 0){
		printf("accept error.\n");
		return 1;
	}
	
	close(socket_desc);

	return 0;
}