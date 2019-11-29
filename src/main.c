#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr()
#include <unistd.h>

#include "http.h"
#include "server.h"
#include "rio.h"

int main(int argc, char* argv[]) {

/* ---- lyf test ---- */
	struct sockaddr_in client;
	socklen_t new_socket_len;
	int socket_desc, new_socket;
	char recv[2000];
	char *reply = "HTTP/1.1 200 ok";

	socket_desc = create_server();

	int count=0;
	//accept and send data
	while( (new_socket = accept(socket_desc, (struct sockaddr*)&client, &new_socket_len )) ) {
//	    if(new_socket<0)
//            continue;
	    int statue_code=0;
		read_http(new_socket,&statue_code);

//		Rio *rio = newRio(new_socket);
//		ssize_t n;
//		printf("recv data: \n");
//		while((n = rio->readline(rio, recv, 2000)) != 0){
//			printf("%d %s\n",n, recv);
//		}
//		printf("\n");
//
//		n = rio->writen(rio, reply, strlen(reply));
//		printf("send num: %d\n", n);
//		freeRio(rio);
//		printf("\n\n");
	}
	// //start_serv();
		
	// request_message r;
	// int error;
	// //测试部分 
	// char req[]="GET /chapter17/user.html HTTP/1.1\r\nAccept-Language:zh-CsN\r\nAccept-Language:zh-CddddN\r\nAccept-Language:zh-CN\r\n\r\nname=tom&password=12xz&realName=tomason\r\nname=tom&password=12xz&realName=tomason\r\nname=tom&password=12xrealName=tomason\0";

	// // r=pre_Process(req,&error);	
	// // if(error==__NORMAL__)
	// // {
	// // 	execReq(&r);
	// // }

	close(socket_desc);
/* ---- lyf test ---- */


//	request_message r;
//	int error;
//	//测试部分
//	char req[]="GET /chapter17/user.html HTTP/1.1\r\nAccept-Language:zh-CsN\r\nAccept-Language:zh-CddddN\r\nAccept-Language:zh-CN\r\n\r\nname=tom&password=12xz&realName=tomason\r\nname=tom&password=12xz&realName=tomason\r\nname=tom&password=12xrealName=tomason\0";
//
//	r=pre_Process(req,&error);
//	if(error==__NORMAL__)
//	{
//		execReq(&r);
//	}
//
//	free(r.rl);
//	free(r.rh);

	return 0;
}