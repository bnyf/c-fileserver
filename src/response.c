//
// Created by ubuntu on 11/12/19.
//



#include "response.h"
#include "string.h"
#include "stdio.h"


//Common Tool Area
const uint32_t CHUNKED_PART_SIZE = 1024*128;

const uint32_t RESPONSE_STATUS_LINE_STR_SIZE = 60;
const uint32_t RESPONSE_HEADER_STR_SIZE = 150;

const char* HTTP_VERSION1_1 = "1.1";
const char* HTTP_VERSION1_0 = "1.0";
const char* HTTP_VERSION2_0 = "2.0";

const uint32_t BAD_REQUEST_CODE = 400;
const char* BAD_REQUEST_WORD = "Bad Request";

const uint32_t NOT_FOUND_CODE = 404;
const char* NOT_FOUND_WORD = "Not Found";

const uint32_t OK_CODE = 200;
const char* OK_WORD = "OK";

const uint32_t MULTI_CHOICES_CODE = 300;
const char* MULTI_CHOICES_WORD = "Multiple Choices";

const uint32_t INTERNAL_SERVER_ERROR_CODE = 500;
const char* INTERNAL_SERVER_ERROR_WORD = "Internal Server Error";

const char* CONTENT_LENGTH_KEY = "Content-Length:";

const char* CONTENT_TYPE_KEY = "Content-Type:";
const char* CONTENT_TYPE_VALUE_FILE = "application/octet-stream";

const char* ACCEPT_RANGES_KEY = "Accept-Ranges:";
const char* ACCEPT_RANGES_VALUES_BYTES = "bytes";

const char* TRANSFER_ENCODING_KEY = "Transfer-Encoding:";
const char* TRANSFER_ENCODING_VALUE_CHUNKED = "chunked";

const char* CONNECTION_KEY = "Connection:";

const char* CONTENT_DISPOSITION_FILE_NAME_KEY = "Content-Disposition: attachment; filename=";

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

    if(responseStatusLine == 0){

        return 0;
    }
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
    else if(statusCode == MULTI_CHOICES_CODE){

        return MULTI_CHOICES_WORD;
    }
    else{

        return "default";
    }

}


void free_ResponseStatusLine(ResponseStatusLine* responseStatusLine){

    if(responseStatusLine == 0){

        return;
    }
    free(responseStatusLine);

}


//ResponseHeader Area

static void zero_ResponseHeader(ResponseHeader* responseHeader){

    responseHeader->Content_Length = -1;
    responseHeader->Accept_Ranges = 0;
    responseHeader->Transfer_Encoding = 0;
    responseHeader->Content_Type = 0;
    responseHeader->Connection = 0;
}

void init_ResponseHeader(ResponseHeader* responseHeader,const char* Content_Type,int32_t Content_Length,const char* Accept_Ranges,const char* Transfer_Encoding,const char* Connection){

    zero_ResponseHeader(responseHeader);
    responseHeader->Content_Type = Content_Type;
    responseHeader->Content_Length = Content_Length;
    responseHeader->Accept_Ranges = Accept_Ranges;
    responseHeader->Transfer_Encoding = Transfer_Encoding;
    responseHeader->Connection = Connection;

}

ResponseHeader* new_ResponseHeader(const char* Content_Type,int32_t Content_Length,const char* Accept_Ranges,const char* Connection){

    ResponseHeader* responseHeader = malloc(sizeof(ResponseHeader));
    zero_ResponseHeader(responseHeader);
    responseHeader->Content_Type = Content_Type;
    responseHeader->Content_Length = Content_Length;
    responseHeader->Accept_Ranges = Accept_Ranges;
    responseHeader->Connection = Connection;
    return responseHeader;

}
ResponseHeader* new_ResponseHeader1(const char* Content_Type,const char* Transfer_Encoding,const char* Connection){

    ResponseHeader* responseHeader = malloc(sizeof(ResponseHeader));
    zero_ResponseHeader(responseHeader);
    responseHeader->Content_Type = Content_Type;
    responseHeader->Transfer_Encoding = Transfer_Encoding;
    responseHeader->Connection = Connection;

    return responseHeader;

}
ResponseHeader* new_onErrorHeader(const char* Connection){

    ResponseHeader* responseHeader = malloc(sizeof(ResponseHeader));
    zero_ResponseHeader(responseHeader);
    responseHeader->Connection = Connection;

    return responseHeader;

}

char* generateResponseHeaderStr(ResponseHeader* responseHeader){

    if(responseHeader == 0){

        return 0;
    }
    char* headerStr = malloc(sizeof(char)*(RESPONSE_HEADER_STR_SIZE+1));
    headerStr[0] = 0;
    if(responseHeader->Content_Type){

        strcat(headerStr,CONTENT_TYPE_KEY);
        strcat(headerStr,responseHeader->Content_Type);
        strcat(headerStr,"\r\n");
    }

    if(responseHeader->Content_Length >= 0){

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
    if(responseHeader->Connection){

        strcat(headerStr,CONNECTION_KEY);
        strcat(headerStr,responseHeader->Connection);
        strcat(headerStr,"\r\n");
    }
    strcat(headerStr,"\r\n");
    return headerStr;

}

void free_ResponseHeader(ResponseHeader* responseHeader){

    if(responseHeader == 0){

        return;
    }
    free(responseHeader);

}

//ResponseBody Area

void init_ResponseBody(ResponseBody* responseBody,char* content,uint32_t contentLength){

    responseBody->content = content;
    responseBody->contentLength = contentLength;

}

ResponseBody* new_ResponseBody(char* content,uint32_t contentLength){

    ResponseBody* responseBody = malloc(sizeof(responseBody));
    responseBody->content = content;
    responseBody->contentLength = contentLength;
    return responseBody;
}

char* generateResponseBodyStr(ResponseBody* responseBody){

    if(responseBody == 0){

        return 0;
    }
    return responseBody->content;

}

void free_ResponseBody(ResponseBody* responseBody){

    if(responseBody == 0){

        return;
    }

    if(responseBody -> content != 0){

        free(responseBody->content);
    }

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

char* generateResponseStr(Response* response,uint32_t* responseLength){

    if(response == 0){

        return 0;
    }
    char* statusLineStr = 0;
    char* responseHeaderStr = 0;
    char* responseBodyStr = 0;

    if(response->statusLine != 0){

        statusLineStr = generateStatusLineStr(response->statusLine);
    }
    if(response->responseHeader != 0){

        responseHeaderStr = generateResponseHeaderStr(response->responseHeader);
    }
    if(response->responseBody != 0){

        responseBodyStr = generateResponseBodyStr(response->responseBody);
    }

    uint32_t  bodyLength = 0;
    if(responseBodyStr != 0){

        bodyLength = response->responseBody->contentLength;

    }

    uint32_t length = RESPONSE_STATUS_LINE_STR_SIZE + RESPONSE_HEADER_STR_SIZE + bodyLength + 1;
    char* res = malloc(sizeof(char)*length);
    res[0] = 0;

    if(statusLineStr != 0){

        strcat(res,statusLineStr);
    }
    if(responseHeaderStr != 0){

        strcat(res,responseHeaderStr);
    }
    if(statusLineStr != 0 && responseHeaderStr == 0){

        strcat(res,"\r\n");
    }

    uint32_t lineAndHeaderLength = strlen(res);
    uint32_t i = 0;
    for(;i < bodyLength;++i){

        res[i+lineAndHeaderLength] = responseBodyStr[i];
    }
    res[i+lineAndHeaderLength] = 0;

    if(responseLength != 0){

        *responseLength = i + lineAndHeaderLength;
    }

    free(statusLineStr);
    free(responseHeaderStr);

    return res;

}

void free_Response(Response* response){

    if(response == 0){

        return;
    }
    if(response->statusLine != 0){

        free_ResponseStatusLine(response->statusLine);
    }
    if(response->responseHeader != 0){

        free_ResponseHeader(response->responseHeader);
    }
    if(response->responseBody != 0){

        free_ResponseBody(response->responseBody);
    }

    free(response);


}



char* generateResponseByStatusCode(uint32_t statusCode,uint32_t* length){

    ResponseStatusLine* responseStatusLine = new_ResponseStatusLine(HTTP_VERSION1_1,statusCode);
    ResponseHeader* responseHeader;
    if(statusCode == MULTI_CHOICES_CODE){

        responseHeader = new_ResponseHeader(0,0,0,"close");
    }
    else if(statusCode == BAD_REQUEST_CODE || statusCode == INTERNAL_SERVER_ERROR_CODE){

        responseHeader = new_ResponseHeader(0,0,0,"keep-alive");
    }
    else{
        responseHeader = new_ResponseHeader(0,0,0,0);
    }

    Response* response = new_Response(responseStatusLine,responseHeader,0);


    char* res = generateResponseStr(response,length);

    free_Response(response);

    return res;

}


