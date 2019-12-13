//
// Created by lwx on 11/12/19.
//

#ifndef FILEUPANDDOWN_FILEPRESENTER_H
#define FILEUPANDDOWN_FILEPRESENTER_H

#include <stdint.h>
#include "http.h"



char* generateFullFileDownLoadResponse(char* filePath,request_head* requestHeader,uint32_t* responseLength);

char* generateFullFileUpLoadResponseWithParseBody(char* filePath,char* content,char* boundary,request_head* requestHeader,uint32_t* responseLength);

char* generateFullFileUpLoadResponse(char* filePath,char* content,request_head* requestHeader,uint32_t* responseLength);

int32_t doChunkedFileDownLoadResponse(char* filePath,Rio *rio,request_head* requestHeader);

int32_t doChunkedFileUpLoadResponseWithParseBody(char* filePath,Rio* rio,request_head* requestHeader);

//int32_t doChunkedFileUpLoadResponse(char* filePath,char* content);


//currently unused
char* generateRangeFileDownLoadResponse(char* filePath,uint32_t start,uint32_t end,request_head* requestHeader);

char* generateRangeFileUpLoadResponse(char* filePath,char* content,char* boundary,uint32_t start,uint32_t end,request_head* requestHeader);



#endif //FILEUPANDDOWN_FILEPRESENTER_H
