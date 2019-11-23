//
// Created by lwx on 11/12/19.
//

#include "stdio.h"
#include "response.h"
#include "string.h"
#include "sys/socket.h"


static uint32_t getfileLength(FILE* file){

    uint32_t res = -1;
    fseek(file,0,SEEK_END);
    res = ftell(file);
    fseek(file,0,SEEK_SET);
    return res;

}

static char* onFileNotFound(){

    ResponseStatusLine statusLine;
    statusLine.version = HTTP_VERSION1_1;
    statusLine.statusCode = NOT_FOUND_CODE;
    statusLine.statusWord = getStatusWord(NOT_FOUND_CODE);
    char* statusLineStr = generateStatusLineStr(&statusLine);

    return statusLineStr;

}

static char* onInternalServerError(){

    ResponseStatusLine statusLine;
    statusLine.version = HTTP_VERSION1_1;
    statusLine.statusCode = INTERNAL_SERVER_ERROR_CODE;
    statusLine.statusWord = getStatusWord(INTERNAL_SERVER_ERROR_CODE);
    char* statusLineStr = generateStatusLineStr(&statusLine);

    return statusLineStr;

}

char* generateFullFileDownLoadResponse(char* filePath){

    FILE* file = fopen(filePath,"rb");
    if(file == 0){

        return onFileNotFound();

    }
    else{

        uint32_t fileLength = getfileLength(file);
        char* fileContent = malloc(sizeof(char)*(fileLength+1));
        uint32_t  size = fread(fileContent, sizeof(char),fileLength,file);
        if(ferror(file)){

            fclose(file);
            return onInternalServerError();
        }
        fileContent[size] = 0;
        ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,OK_CODE);
        ResponseHeader* responseHeader = new_ResponseHeader(CONTENT_TYPE_VALUE_FILE,fileLength,ACCEPT_RANGES_VALUES_BYTES);
        ResponseBody* responseBody = new_ResponseBody(fileContent);
        Response* response = new_Response(responseStatusLine,responseHeader,responseBody);
        char* res = generateResponseStr(response);
        free_Response(response);

        fclose(file);
        return res;

    }


}


static int32_t getBoundary(char** contentPtr,char* start_template,char* end_template,int start_template_length,int end_template_length,
        char** fileName,char** fileContent,uint32_t* fileContentLength,int32_t* status){

    *status = 0;
    char* content = *contentPtr;
    if(content == 0){

        return 0;
    }

    char* startIndex = strstr(content,start_template);
    char* endIndex = strstr(content,end_template);
    uint32_t contentLength = strlen(content);
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

    char* temp = strstr(startIndex,"\r\n\r\n");
    if(temp == 0){

        *status = 1;
        return 1;
    }
    temp += sizeof(char)*4;
    char* temp2 = strstr(temp,start_template);
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

    char* temp1 = strstr(startIndex,"filename");
    if(temp1 == 0){

        *status = 1;
        return 1;
    }
    temp1 += sizeof(char)*10;
    if(*(temp1-1* sizeof(char)) != '"' || *(temp1-2* sizeof(char)) != '='){

        *status = 1;
        return 1;
    }
    char* temp3 = strstr(temp1,"\"");
    if(temp3 == 0){

        *status = 1;
        return 1;
    }
    *temp3 = 0;
    *fileName = temp1;

    return 1;

}

static char* onBoundaryParseError(){

    ResponseStatusLine responseStatusLine;
    init_ResponseStatusLine(&responseStatusLine,HTTP_VERSION1_1,BAD_REQUEST_CODE);
    return generateStatusLineStr(&responseStatusLine);

}


char* generateFullFileUpLoadResponseWithParseBody(char* path,char* content,char* boundary){

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
    while(getBoundary(&content,start_template,end_template,start_template_length,end_template_length,&fileName,&fileContent,&fileContentLength,&status)){

        if(status){

            return onBoundaryParseError();
        }
        FILE* file = fopen(fileName,"wb");

        fwrite(fileContent, sizeof(char),fileContentLength,file);
        if(ferror(file)){
            fclose(file);
            return onInternalServerError();
        }

        fclose(file);
    }

    ResponseStatusLine responseStatusLine;
    init_ResponseStatusLine(&responseStatusLine,HTTP_VERSION1_1,OK_CODE);

    return generateStatusLineStr(&responseStatusLine);

}


static void generateChunkedPart(char* buffer,uint32_t chunkedSize,char* chunkedContent,int32_t whetherEOF){

    buffer[0] = 0;
    if(chunkedSize > 0){

        char temp[50];
        sprintf(temp,"%x",chunkedSize);
        strcat(buffer,temp);
        strcat(buffer,"\r\n");
        strcat(buffer,chunkedContent);
        strcat(buffer,"\r\n");

    }

    if(whetherEOF){

        strcat(buffer,"0\r\n");
    }

}

void doChunkedFileDownLoadResponse(char* filePath,int32_t socketNum){

    FILE* file = fopen(filePath,"rb");
    if(file == 0){

        char* res = onFileNotFound();
        send(socketNum,res,strlen(res),0);
        free(res);
        return;

    }

    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,OK_CODE);
    ResponseHeader* responseHeader = new_ResponseHeader1(CONTENT_TYPE_VALUE_FILE,TRANSFER_ENCODING_VALUE_CHUNKED);
    char temp[sizeof(char)*(CHUNKED_PART_SIZE+1)];
    char buffer[sizeof(char)*(CHUNKED_PART_SIZE+15)];
    uint32_t size = fread(temp, sizeof(char),CHUNKED_PART_SIZE,file);
    if(ferror(file)){

        char* res = onInternalServerError();
        send(socketNum,res,strlen(res),0);
        free(res);
        fclose(file);
        return;
    }
    temp[size] = 0;

    if(feof(file)){

        generateChunkedPart(buffer,size,temp,1);

    }
    else{

//        if(size == 0){
//
//            char* res = onInternalServerError();
//            send(socketNum,res,strlen(res),0);
//            free(res);
//            fclose(file);
//            return;
//
//        }
        generateChunkedPart(buffer,size,temp,0);

    }

    ResponseBody* responseBody = new_ResponseBody(buffer);
    Response* response = new_Response(responseStatusLine,responseHeader,responseBody);
    char* res = generateResponseStr(response);
    send(socketNum,res,strlen(res),0);
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

            res = onInternalServerError();
            send(socketNum,res,strlen(res),0);
            free(res);
            fclose(file);
            return;
        }
        temp[size] = 0;
        if(feof(file)){

            generateChunkedPart(buffer,size,temp,1);
            send(socketNum,buffer,strlen(buffer),0);
            fclose(file);
            return;
        }
        else{

//            if(size == 0){
//
//                res = onInternalServerError();
//                send(socketNum,res,strlen(res),0);
//                free(res);
//                fclose(file);
//                return;
//
//            }
            generateChunkedPart(buffer,size,temp,0);
            send(socketNum,buffer,strlen(buffer),0);

        }


    }




}


void doChunkedFileUpLoadResponseWithParseBody(char* filePath,int32_t socketNum){

    uint32_t size = 0;
    FILE* file = fopen(filePath,"wb");
    if(file == 0){

        char* res = onFileNotFound();
        send(socketNum,res,strlen(res),0);
        free(res);
        return;
    }
    char temp[size+1];
    FILE* socketFile = fdopen(socketNum,"rb");
    while(1){

        fscanf(socketFile,"%x\r\n",&size);
        if(size == 0){

            break;
        }

        uint32_t length = fread(temp, sizeof(char),size,socketFile);
        fgetc(socketFile);
        fgetc(socketFile);

        fwrite(temp, sizeof(char),length,file);
        if(ferror(file)){

            char* res = onInternalServerError();
            send(socketNum,res,strlen(res),0);
            free(res);
            fclose(file);
            return;

        }


    }

    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,OK_CODE);
    char* res = generateStatusLineStr(responseStatusLine);
    send(socketNum,res,strlen(res),0);
    free(res);
    free_ResponseStatusLine(responseStatusLine);
    fclose(file);

}

int32_t doFileUpLoad(char* filePath,char* content){

    FILE* file = fopen(filePath,"wb");

    uint32_t size = strlen(content);
    fwrite(content, sizeof(char), sizeof(char)*size,file);
    if(ferror(file)){

        fclose(file);
        return 1;
    }
    fclose(file);
    return 0;

}

char* generateFullFileUpLoadResponse(char* filePath,char* content){


    if(doFileUpLoad(filePath,content)){

        return onInternalServerError();
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




