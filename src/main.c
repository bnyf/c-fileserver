#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr()
#include <unistd.h>

#include "http.h"
#include "server.h"

int main(int argc, char* argv[]) {

	//start_serv();
		
	request_message r;
	int error;
	//测试部分 
	char req[]="GET /chapter17/user.html HTTP/1.1\r\nAccept-Language:zh-CsN\r\nAccept-Language:zh-CddddN\r\nAccept-Language:zh-CN\r\n\r\nname=tom&password=12xz&realName=tomason\r\nname=tom&password=12xz&realName=tomason\r\nname=tom&password=12xrealName=tomason\0";

	// r=pre_Process(req,&error);	
	// if(error==__NORMAL__)
	// {
	// 	execReq(&r);
	// }

	free(r.rl);
	free(r.rh);

	return 0;
}