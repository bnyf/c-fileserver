#include <stdio.h>
#include<stdlib.h>
#include"http.h"
int main(int argc, char* argv[]){

	request_message r;
	int error;
	//测试部分 
	char req[]="GET /chapter17/user.html HTTP/1.1\r\nAccept-Language:zh-CsN\r\nAccept-Language:zh-CddddN\r\nAccept-Language:zh-CN\r\n\r\nname=tom&password=12xz&realName=tomason\r\nname=tom&password=12xz&realName=tomason\r\nname=tom&password=12xrealName=tomason\0";

	r=pre_Process(req,&error);	
	if(error==__NORMAL__)
	{
		execReq(&r);
	}

	free(r.rl);
	free(r.rh);

	return 0;
}