/*
 * -> gcc -o client client.c && ./client
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h> // for close
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr()
#include <sys/errno.h>
int main(int argc, char* argv[]) {
	int socket_desc;
	struct sockaddr_in server;
	char buf[2000];

	// create socket
	if( (socket_desc = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //IPv4, TCP, IP
		printf("create socket error.\n");
		return 1;
	}

	//init server
	server.sin_family = AF_INET; //IPv4
	server.sin_addr.s_addr = inet_addr("127.0.0.1"); // baidu.com
	server.sin_port = htons(8080);

	//connect to remote server
	if(connect(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("connect error.\n");
		return 1;
	}
	printf("connected.\n");

	//send data
	char *message = "GET /chapter17/user.html HTTP/1.1\r\nAccept-Language:zh-CsN\r\nAccept-Language:zh-CddddN\r\nAccept-Language:zh-CN\r\n\r\n";
	int len = strlen(message);
	int tot = 0;
	int n = 0;
	printf("need send: %d\n",len);
	while(tot < len)  {
		n = send(socket_desc, message, len, 0);
		if(n == -1 && errno != EINTR){
			printf("sned error.\n");
			return 1;
		}
		tot +=n;
		printf("data send: %d\n",tot);
	}

	// receive data
	if( ( n = read(socket_desc, buf, 2000) ) >= 0 ){
		if(n == -1 && errno != EINTR){
			printf("sned error.\n");
			return 1;
		}
		if(n != 0) {
            buf[n] = 0;
            printf("data recv: %d\n", n);
            printf("%s\n", buf);
        }
	}

	close(socket_desc);

	return 0;
}