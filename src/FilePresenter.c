//
// Created by lwx on 11/12/19.
//

#include "stdio.h"
#include "response.h"
#include "string.h"
#include "sys/socket.h"
#include "FilePresenter.h"
#include "file.h"

static uint32_t getfileLength(FILE* file){

    uint32_t res = -1;
    fseek(file,0,SEEK_END);
    res = ftell(file);
    fseek(file,0,SEEK_SET);
    return res;

}

static char* onFileNotFound(request_head* requestHeader,uint32_t* length){

    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,NOT_FOUND_CODE);
    Response* response = 0;
    if(requestHeader != 0 && requestHeader->connection != 0){

        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,requestHeader->connection);
        response = new_Response(responseStatusLine,responseHeader,0);
    }
    else{
        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,0);
        response = new_Response(responseStatusLine,responseHeader,0);
    }


    char* res = generateResponseStr(response,length);
    free_Response(response);

    return res;

}

static char* onInternalServerError(request_head* requestHeader,uint32_t* responseLength){


    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,INTERNAL_SERVER_ERROR_CODE);
    Response* response = 0;
    if(requestHeader != 0 && requestHeader->connection != 0){

        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,requestHeader->connection);
        response = new_Response(responseStatusLine,responseHeader,0);
    }
    else{
        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,0);
        response = new_Response(responseStatusLine,responseHeader,0);
    }


    char* res = generateResponseStr(response,responseLength);
    free_Response(response);
    return res;

}

char* generateFullFileDownLoadResponse(char* filePath,request_head* requestHeader,uint32_t* responseLength){

    FILE* file = openFileWithCustomedPath(filePath,"rb");
    if(file == 0){

        return onFileNotFound(requestHeader,responseLength);

    }
    else{

        uint32_t fileLength = getfileLength(file);
        char* fileContent = malloc(sizeof(char)*(fileLength+1));
        uint32_t  size = fread(fileContent, sizeof(char),fileLength,file);
        if(ferror(file)){

            fclose(file);
            return onInternalServerError(requestHeader,responseLength);
        }
        fileContent[size] = 0;
        ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,OK_CODE);
        ResponseHeader* responseHeader = 0;
        if(requestHeader == 0 || requestHeader->connection == 0){

            responseHeader = new_ResponseHeader(CONTENT_TYPE_VALUE_FILE,size,ACCEPT_RANGES_VALUES_BYTES,0);
        }
        else{

            responseHeader = new_ResponseHeader(CONTENT_TYPE_VALUE_FILE,size,ACCEPT_RANGES_VALUES_BYTES,requestHeader->connection);
        }

        ResponseBody* responseBody = new_ResponseBody(fileContent,size);
        Response* response = new_Response(responseStatusLine,responseHeader,responseBody);
        char* res = generateResponseStr(response,responseLength);
        free_Response(response);

        fclose(file);
        return res;

    }


}

static char* findStr(char* str1,const char* str2,uint32_t bodyLength){

    for(uint32_t i = 0; i < bodyLength; ++i){

        uint32_t temp = i;
        uint32_t temp1 = i;
        uint32_t j = 0;
        for(; str2[j] != 0; ++j){

            if(str1[temp] != str2[j]){

                break;
            }
            ++temp;

        }
        if(str2[j] == 0){

            return &str1[temp1];
        }

    }

    return 0;

}

static int32_t getBoundary(char** contentPtr,char* start_template,char* end_template,int start_template_length,int end_template_length,
        char** fileName,char** fileContent,uint32_t* fileContentLength,int32_t* status,uint32_t bodyLength,const char* startPtr){

    *status = 0;
    char* content = *contentPtr;
    if(content == 0){

        return 0;
    }

    char* startIndex = findStr(content,start_template,bodyLength);
    char* endIndex = findStr(content,end_template,bodyLength);
    uint32_t contentLength = bodyLength - (content - startPtr);
    if(contentLength == 0){

        return 0;
    }

    if(startIndex == 0 && endIndex != 0){

        *status = 1;
        return 1;
    }
    else if(startIndex != 0 && endIndex == 0){

        *status = 1;
        return 1;
    }
    else if(startIndex == 0 && endIndex == 0 && contentLength != 0){

        *status = 1;
        return 1;
    }

    startIndex += sizeof(char)*(start_template_length+2);

    char* temp = findStr(startIndex,"\r\n\r\n",bodyLength);
    if(temp == 0){

        *status = 1;
        return 1;
    }
    temp += sizeof(char)*4;
    char* temp2 = findStr(temp,start_template,bodyLength);
    if(temp2 == endIndex){

        *(endIndex-2* sizeof(char)) = 0;
        *fileContentLength = (endIndex-2* sizeof(char)) - temp;
        *contentPtr = 0;

    }
    else{

        *(temp2-2* sizeof(char)) = 0;
        *fileContentLength = (temp2-2* sizeof(char)) - temp;
        *contentPtr = temp2;
    }
    *fileContent = temp;

    char* temp1 = findStr(startIndex,"filename",bodyLength);
    if(temp1 == 0){

        *status = 1;
        return 1;
    }
    temp1 += sizeof(char)*10;
    if(*(temp1-1* sizeof(char)) != '"' || *(temp1-2* sizeof(char)) != '='){

        *status = 1;
        return 1;
    }
    char* temp3 = findStr(temp1,"\"",bodyLength);
    if(temp3 == 0){

        *status = 1;
        return 1;
    }
    *temp3 = 0;
    *fileName = temp1;

    return 1;

}

static char* onBoundaryParseError(request_head* requestHeader,uint32_t* length){


    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,BAD_REQUEST_CODE);
    Response* response = 0;
    if(requestHeader != 0 && requestHeader->connection != 0){

        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,requestHeader->connection);
        response = new_Response(responseStatusLine,responseHeader,0);
    }
    else{
        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,0);
        response = new_Response(responseStatusLine,responseHeader,0);
    }


    char* res = generateResponseStr(response,length);
    free_Response(response);

    return res;

}


char* generateFullFileUpLoadResponseWithParseBody(char* path,char* content,char* boundary,request_head* requestHeader,uint32_t* responseLength){

    //FILE* file = fopen(filePath,"wb");


    uint32_t boundaryLength = strlen(boundary);
    char* start_template = malloc(sizeof(char)*(boundaryLength+3));
    char* end_template = malloc(sizeof(char)*(boundaryLength+5));
    start_template[0] = 0;
    end_template[0] = 0;
    strcat(start_template,"--");
    strcat(start_template,boundary);

    strcat(end_template,"--");
    strcat(end_template,boundary);
    strcat(end_template,"--");

    uint32_t start_template_length = strlen(start_template);
    uint32_t end_template_length = strlen(end_template);

    char* fileName;
    char* fileContent;
    uint32_t fileContentLength;
    int32_t status;
    while(getBoundary(&content,start_template,end_template,start_template_length,end_template_length,&fileName,&fileContent,&fileContentLength,&status,
            requestHeader->content_length,content)){

        if(status){

            return onBoundaryParseError(requestHeader,responseLength);
        }
        FILE* file = openFileWithCustomedPath(fileName,"wb");

        fwrite(fileContent, sizeof(char),fileContentLength,file);
        if(ferror(file)){
            fclose(file);
            return onInternalServerError(requestHeader,responseLength);
        }

        fclose(file);
    }

    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,OK_CODE);
    Response* response = 0;
    if(requestHeader != 0 && requestHeader->connection != 0){

        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,requestHeader->connection);
        response = new_Response(responseStatusLine,responseHeader,0);
    }
    else{
        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,0);
        response = new_Response(responseStatusLine,responseHeader,0);
    }

    char* res = generateResponseStr(response,responseLength);
    free_Response(response);

    return res;

}


static void generateChunkedPart(char* buffer,uint32_t chunkedSize,const char* chunkedContent,int32_t whetherEOF,uint32_t* chunkTotalSize){

    buffer[0] = 0;
    if(chunkedSize > 0){

        char temp[50];
        sprintf(temp,"%x",chunkedSize);
        *chunkTotalSize = strlen(temp) + chunkedSize + sizeof(char)*4;
        strcat(buffer,temp);
        strcat(buffer,"\r\n");
        uint32_t i = strlen(buffer);
        uint32_t j = 0;
        for(; j < chunkedSize; ++j){

            buffer[j+i] = chunkedContent[j];
        }
        buffer[j+i] = '\r';
        buffer[j+i+1] = '\n';

    }

    if(whetherEOF){

        strcat(buffer,"0\r\n");
        *chunkTotalSize = sizeof(char)*3;
    }

}

void doChunkedFileDownLoadResponse(char* filePath,Rio *rio,request_head* requestHeader){

    FILE* file = openFileWithCustomedPath(filePath,"rb");
    if(file == 0){

        uint32_t length = 0;
        char* res = onFileNotFound(requestHeader,&length);
        rio->writen(rio,res,length);
        free(res);
        return;

    }



    char temp[sizeof(char)*(CHUNKED_PART_SIZE+1)];
    char buffer[sizeof(char)*(CHUNKED_PART_SIZE+15)];
    uint32_t size = fread(temp, sizeof(char),CHUNKED_PART_SIZE,file);
    if(ferror(file)){

        uint32_t length = 0;
        char* res = onInternalServerError(requestHeader,&length);
        rio->writen(rio,res,length);
        free(res);
        fclose(file);
        return;
    }
    temp[size] = 0;

    uint32_t chunkSize = 0;
    if(feof(file)){

        generateChunkedPart(buffer,size,temp,1,&chunkSize);

    }
    else{

        generateChunkedPart(buffer,size,temp,0,&chunkSize);

    }

    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,OK_CODE);
    ResponseHeader* responseHeader = 0;
    if(requestHeader != 0 && requestHeader->connection != 0){

        responseHeader = new_ResponseHeader1(CONTENT_TYPE_VALUE_FILE,TRANSFER_ENCODING_VALUE_CHUNKED,requestHeader->connection);
    }
    else{

        responseHeader = new_ResponseHeader1(CONTENT_TYPE_VALUE_FILE,TRANSFER_ENCODING_VALUE_CHUNKED,0);
    }
    ResponseBody* responseBody = new_ResponseBody(buffer,chunkSize);
    Response* response = new_Response(responseStatusLine,responseHeader,responseBody);
    uint32_t length = 0;
    char* res = generateResponseStr(response,&length);
    rio->writen(rio,res,length);
    free_ResponseStatusLine(responseStatusLine);
    free_ResponseHeader(responseHeader);
    free(responseBody);
    free(response);
    free(res);

    if(feof(file)){

        fclose(file);
        return;
    }

    while(1){

        size = fread(temp, sizeof(char),CHUNKED_PART_SIZE,file);
        if(ferror(file)){

            uint32_t lengthTemp = 0;
            res = onInternalServerError(requestHeader,&lengthTemp);
            rio->writen(rio,res,lengthTemp);

            free(res);
            fclose(file);
            return;
        }
        temp[size] = 0;
        if(feof(file)){

            uint32_t lengthTemp = 0;
            generateChunkedPart(buffer,size,temp,1,&lengthTemp);
            rio->writen(rio,buffer,lengthTemp);
            fclose(file);
            return;
        }
        else{

            uint32_t lengthTemp = 0;
            generateChunkedPart(buffer,size,temp,0,&lengthTemp);
            rio->writen(rio,buffer,lengthTemp);

        }


    }




}

static uint32_t multiply(uint32_t base,uint32_t exp){

    uint32_t res = 1;
    for(uint32_t i = 0; i < exp; ++i){

        res *= base;
    }

    return res;

}
static uint32_t charToTen(char ch){

    if(ch >= '0' && ch <= '9'){

        return ch - '0';
    }
    else if(ch >= 'a' && ch <= 'f'){

        return ch - 'a' + 10;
    }
    else if(ch >= 'A' && ch <= 'F'){

        return ch - 'A' + 10;
    }
    else{

        return -1;
    }

}
static uint32_t parseHexLength(char* str){

    uint32_t res = 0;
    int length = 0;
    for(;str[length] != 0; ++length);

    for(int i = length - 1; i >= 0; --i){

        if((str[i] >= '0' && str[i] <= '9')|| (str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F')){

            res += charToTen(str[i])*multiply(16,length-i-1);
        }
        else{

            return -1;
        }
    }

    return res;

}

static char* onFileLengthParseError(request_head* requestHeader,uint32_t* length){

    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,BAD_REQUEST_CODE);
    Response* response = 0;
    if(requestHeader != 0 && requestHeader->connection != 0){

        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,requestHeader->connection);
        response = new_Response(responseStatusLine,responseHeader,0);
    }
    else{
        ResponseHeader* responseHeader = new_ResponseHeader(0,0,0,0);
        response = new_Response(responseStatusLine,responseHeader,0);
    }


    char* res = generateResponseStr(response,length);
    free_Response(response);

    return res;

}
void doChunkedFileUpLoadResponseWithParseBody(char* filePath,Rio* rio,request_head* requestHeader){

    uint32_t size = 0;
    FILE* file = openFileWithCustomedPath(filePath,"wb");
    if(file == 0){

        uint32_t length = 0;
        char* res = onFileNotFound(requestHeader,&length);
        rio->writen(rio,res,length);
        free(res);
        return;
    }


    while(1){

        char temp[1024];
        rio->readline(rio,temp,1024);
        uint32_t fileLength = parseHexLength(temp);
        if(fileLength == -1){
            uint32_t  length = 0;
            char* res = onFileLengthParseError(requestHeader,&length);
            rio->writen(rio,res,length);
            free(res);
            fclose(file);
            return;
        }
        if(fileLength == 0){

            break;
        }

        char buffer[fileLength+1];
        uint32_t realLength = rio->readn(rio,buffer,fileLength);
        if(realLength != fileLength){

            uint32_t  length = 0;
            char* res = onFileLengthParseError(requestHeader,&length);
            rio->writen(rio,res,length);
            free(res);
            fclose(file);
            return;

        }
        char qtemp1[10];
        char qtemp2[10];

        if(rio->readn(rio,qtemp1,1) != 1 || rio->readn(rio,qtemp2,1) != 1 || qtemp1[0] !='\r' || qtemp2[0] != '\n'){

            uint32_t  length = 0;
            char* res = onFileLengthParseError(requestHeader,&length);
            rio->writen(rio,res,length);
            free(res);
            fclose(file);
            return;
        }

        fwrite(buffer, sizeof(char),realLength,file);
        if(ferror(file)){

            uint32_t lengthTemp = 0;
            char* res = onInternalServerError(requestHeader,&lengthTemp);
            rio->writen(rio,res,lengthTemp);
            free(res);
            fclose(file);
            return;
        }

    }


    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,OK_CODE);
    ResponseHeader* responseHeader;
    if(requestHeader != 0 && requestHeader->connection != 0){

        responseHeader = new_ResponseHeader(0,0,0,requestHeader->connection);
    }
    else{

        responseHeader = new_ResponseHeader(0,0,0,0);
    }
    Response* response = new_Response(responseStatusLine,responseHeader,0);
    uint32_t length = 0;
    char* res = generateResponseStr(response,&length);
    rio->writen(rio,res,length);
    free(res);
    free_Response(response);
    fclose(file);

}

int32_t doFileUpLoad(char* filePath,char* content){

    FILE* file = openFileWithCustomedPath(filePath,"wb");

    uint32_t size = strlen(content);
    fwrite(content, sizeof(char), sizeof(char)*size,file);
    if(ferror(file)){

        fclose(file);
        return 1;
    }
    fclose(file);
    return 0;

}

char* generateFullFileUpLoadResponse(char* filePath,char* content,request_head* requestHeader,uint32_t* responseLength){


    if(doFileUpLoad(filePath,content)){

        return onInternalServerError(requestHeader,responseLength);
    }
    else{

        ResponseStatusLine responseStatusLine;
        init_ResponseStatusLine(&responseStatusLine,HTTP_VERSION1_1,OK_CODE);
        char* res = generateStatusLineStr(&responseStatusLine);
        return res;

    }

}

int32_t doChunkedFileUpLoadResponse(char* filePath,char* content){

    if(doFileUpLoad(filePath,content)){

        return 1;
    }
    else{

        return 0;

    }


}




