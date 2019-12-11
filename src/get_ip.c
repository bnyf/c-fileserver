#include "get_ip.h"
void get_ip(char *local_ip)
{

    struct ifaddrs * ifAddrStruct = NULL;
    void * tmpAddrPtr = NULL;

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct!=NULL)
    {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET)
        {
            tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            if(strcmp(ifAddrStruct->ifa_name,"eth0") == 0 || strcmp(ifAddrStruct->ifa_name,"en0") == 0 ){
                strcpy(local_ip,addressBuffer);
                break;
            }
        }
        ifAddrStruct = ifAddrStruct->ifa_next;
    }
}