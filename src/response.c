//
// Created by ubuntu on 11/12/19.
//



#include "response.h"
#include "string.h"
#include "stdio.h"


//Common Tool Area
const uint32_t CHUNKED_PART_SIZE = 1024;

const uint32_t RESPONSE_STATUS_LINE_STR_SIZE = 60;
const uint32_t RESPONSE_HEADER_STR_SIZE = 100;

const char* HTTP_VERSION1_1 = "1.1";
const char* HTTP_VERSION1_0 = "1.0";
const char* HTTP_VERSION2_0 = "2.0";

const uint32_t BAD_REQUEST_CODE = 400;
const char* BAD_REQUEST_WORD = "Bad Request";

const uint32_t NOT_FOUND_CODE = 404;
const char* NOT_FOUND_WORD = "Not Found";

const uint32_t OK_CODE = 200;
const char* OK_WORD = "OK";

const uint32_t INTERNAL_SERVER_ERROR_CODE = 500;
const char* INTERNAL_SERVER_ERROR_WORD = "Internal Server Error";

const char* CONTENT_LENGTH_KEY = "Content-Length:";

const char* CONTENT_TYPE_KEY = "Content-Type:";
const char* CONTENT_TYPE_VALUE_FILE = "application/octet-stream";

const char* ACCEPT_RANGES_KEY = "Accept-Ranges:";
const char* ACCEPT_RANGES_VALUES_BYTES = "bytes";

const char* TRANSFER_ENCODING_KEY = "Transfer-Encoding:";
const char* TRANSFER_ENCODING_VALUE_CHUNKED = "chunked";


static void intToStr(char* buffer,int32_t num){

    sprintf(buffer,"%d",num);

}


//StatusLine Area


void init_ResponseStatusLine(ResponseStatusLine* responseStatusLine,const char* version,uint32_t statusCode){

    responseStatusLine->version = version;
    responseStatusLine->statusCode = statusCode;
    responseStatusLine->statusWord = getStatusWord(statusCode);

}

ResponseStatusLine* new_ResponseStatusLine(const char* version,uint32_t statusCode){

    ResponseStatusLine* res = malloc(sizeof(ResponseStatusLine));
    res->version = version;
    res->statusCode = statusCode;
    res->statusWord = getStatusWord(statusCode);
    return res;

}

char* generateStatusLineStr(ResponseStatusLine* responseStatusLine){

    char* statusLine = malloc(sizeof(char)*(RESPONSE_STATUS_LINE_STR_SIZE+1));
    statusLine[0] = 0;
    strcat(statusLine,"HTTP/");
    strcat(statusLine,responseStatusLine->version);
    strcat(statusLine," ");
    char temp[sizeof(char)*10];
    intToStr(temp,responseStatusLine->statusCode);
    strcat(statusLine,temp);
    strcat(statusLine," ");
    strcat(statusLine,responseStatusLine->statusWord);
    strcat(statusLine,"\r\n");

    return statusLine;

}

const char* getStatusWord(uint32_t statusCode){

    if(statusCode == OK_CODE){

        return OK_WORD;
    }
    else if(statusCode == NOT_FOUND_CODE){

        return NOT_FOUND_WORD;
    }
    else if(statusCode == INTERNAL_SERVER_ERROR_CODE){

        return INTERNAL_SERVER_ERROR_WORD;
    }
    else if(statusCode == BAD_REQUEST_CODE){

        return BAD_REQUEST_WORD;
    }
    else{

        return "default";
    }

}


void free_ResponseStatusLine(ResponseStatusLine* responseStatusLine){

    free(responseStatusLine);

}


//ResponseHeader Area

static void zero_ResponseHeader(ResponseHeader* responseHeader){

    responseHeader->Content_Length = 0;
    responseHeader->Accept_Ranges = 0;
    responseHeader->Transfer_Encoding = 0;
    responseHeader->Content_Type = 0;

}

void init_ResponseHeader(ResponseHeader* responseHeader,const char* Content_Type,uint32_t Content_Length,const char* Accept_Ranges){

    zero_ResponseHeader(responseHeader);
    responseHeader->Content_Type = Content_Type;
    responseHeader->Content_Length = Content_Length;
    responseHeader->Accept_Ranges = Accept_Ranges;

}

ResponseHeader* new_ResponseHeader(const char* Content_Type,uint32_t Content_Length,const char* Accept_Ranges){

    ResponseHeader* responseHeader = malloc(sizeof(ResponseHeader));
    zero_ResponseHeader(responseHeader);
    responseHeader->Content_Type = Content_Type;
    responseHeader->Content_Length = Content_Length;
    responseHeader->Accept_Ranges = Accept_Ranges;
    return responseHeader;

}
ResponseHeader* new_ResponseHeader1(const char* Content_Type,const char* Transfer_Encoding){

    ResponseHeader* responseHeader = malloc(sizeof(ResponseHeader));
    zero_ResponseHeader(responseHeader);
    responseHeader->Content_Type = Content_Type;
    responseHeader->Transfer_Encoding = Transfer_Encoding;
    return responseHeader;

}


char* generateResponseHeaderStr(ResponseHeader* responseHeader){

    char* headerStr = malloc(sizeof(char)*(RESPONSE_HEADER_STR_SIZE+1));
    headerStr[0] = 0;
    if(responseHeader->Content_Type){

        strcat(headerStr,CONTENT_TYPE_KEY);
        strcat(headerStr,responseHeader->Content_Type);
        strcat(headerStr,"\r\n");
    }

    if(responseHeader->Content_Length > 0){

        strcat(headerStr,CONTENT_LENGTH_KEY);
        char temp[sizeof(char)*15];
        intToStr(temp,responseHeader->Content_Length);
        strcat(headerStr,temp);
        strcat(headerStr,"\r\n");

    }

    if(responseHeader->Accept_Ranges){

        strcat(headerStr,ACCEPT_RANGES_KEY);
        strcat(headerStr,responseHeader->Accept_Ranges);
        strcat(headerStr,"\r\n");

    }

    if(responseHeader->Transfer_Encoding){

        strcat(headerStr,TRANSFER_ENCODING_KEY);
        strcat(headerStr,responseHeader->Transfer_Encoding);
        strcat(headerStr,"\r\n");
    }
    strcat(headerStr,"\r\n");
    return headerStr;

}

void free_ResponseHeader(ResponseHeader* responseHeader){

    free(responseHeader);

}

//ResponseBody Area

void init_ResponseBody(ResponseBody* responseBody,char* content){

    responseBody->content = content;

}

ResponseBody* new_ResponseBody(char* content){

    ResponseBody* responseBody = malloc(sizeof(responseBody));
    responseBody->content = content;
    return responseBody;
}

char* generateResponseBodyStr(ResponseBody* responseBody){

    return responseBody->content;

}

void free_ResponseBody(ResponseBody* responseBody){

    free(responseBody->content);
    free(responseBody);
}

//Response Area

void init_Response(Response* response,ResponseStatusLine* statusLine,ResponseHeader* responseHeader,ResponseBody* responseBody){

    response->statusLine = statusLine;
    response->responseHeader = responseHeader;
    response->responseBody = responseBody;

}

Response* new_Response(ResponseStatusLine* statusLine,ResponseHeader* responseHeader,ResponseBody* responseBody){

    Response* response = malloc(sizeof(Response));
    response->statusLine = statusLine;
    response->responseHeader = responseHeader;
    response->responseBody = responseBody;

    return response;
}

char* generateResponseStr(Response* response){

    char* statusLineStr = generateStatusLineStr(response->statusLine);
    char* responseHeaderStr = generateResponseHeaderStr(response->responseHeader);
    char* responseBodyStr = generateResponseBodyStr(response->responseBody);

    uint32_t  bodyLength = strlen(responseBodyStr);
    uint32_t length = RESPONSE_STATUS_LINE_STR_SIZE + RESPONSE_HEADER_STR_SIZE + bodyLength + 1;
    char* res = malloc(sizeof(char)*length);
    res[0] = 0;
    strcat(res,statusLineStr);
    strcat(res,responseHeaderStr);
    strcat(res,responseBodyStr);
    free(statusLineStr);
    free(responseHeaderStr);

    return res;

}

void free_Response(Response* response){

    free_ResponseStatusLine(response->statusLine);
    free_ResponseHeader(response->responseHeader);
    free_ResponseBody(response->responseBody);
    free(response);


}






