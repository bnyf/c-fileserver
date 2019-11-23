//
// Created by lwx on 11/12/19.
//

#ifndef FILEUPANDDOWN_FILEPRESENTER_H
#define FILEUPANDDOWN_FILEPRESENTER_H

#include <stdint.h>




char* generateFullFileDownLoadResponse(char* filePath);

char* generateFullFileUpLoadResponseWithParseBody(char* filePath,char* content,char* boundary);

char* generateFullFileUpLoadResponse(char* filePath,char* content);

void doChunkedFileDownLoadResponse(char* filePath,int32_t socketNum);

void doChunkedFileUpLoadResponseWithParseBody(char* filePath,int32_t socketNum);

void doChunkedFileUpLoadResponse(char* filePath,char* content);

char* generateRangeFileDownLoadResponse(char* filePath,uint32_t start,uint32_t end);

char* generateRangeFileUpLoadResponse(char* filePath,char* content,char* boundary,uint32_t start,uint32_t end);



#endif //FILEUPANDDOWN_FILEPRESENTER_H
